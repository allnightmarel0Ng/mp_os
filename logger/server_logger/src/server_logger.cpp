#include "../include/server_logger.h"

#ifdef _WIN32
std::map<std::string, std::pair<HANDLE, size_t>> server_logger::_queues_users =
    std::map<std::string, std::pair<HANDLE, size_t>>();
#else
std::map<std::string, std::pair<mqd_t, size_t>> server_logger::_queues_users =
    std::map<std::string, std::pair<mqd_t, size_t>>();
#endif

server_logger::server_logger(
    std::map<std::string, std::set<logger::severity>> const keys)   
{       
    for (auto &[key, set] : keys) {
        if (_queues_users.find(key) == _queues_users.end()) {

#ifdef _WIN32
            HANDLE queue = CreateFileA(
                key.c_str(), GENERIC_WRITE, 0, 
                nullptr, OPEN_EXISTING, 0, nullptr);
            if (queue == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Unable to open the queue\n");
            }            
#else
            mqd_t queue = mq_open(key.c_str(), O_WRONLY, 0644);
            if (queue == (mqd_t)-1) {
                throw std::runtime_error("Unable to open the queue\n");
            }
#endif

            _queues_users[key].first = queue;
            _queues_users[key].second = 1;

            _queues[key].first = queue;
        }
        else {
            _queues_users[key].second++;
            _queues[key].first = _queues_users[key].first;
        } 

        _queues[key].second = set;
    }

#ifdef _WIN32
    _process_id = GetCurrentProcessId();
#else
    _process_id = getpid();
#endif

    _session_id = 0;
}

server_logger::server_logger(server_logger const &other) = default;

server_logger &server_logger::operator=(server_logger const &other) = default;

server_logger::server_logger(server_logger &&other) noexcept = default;

server_logger &server_logger::operator=(
    server_logger &&other) noexcept = default;

server_logger::~server_logger() noexcept
{
    for (auto &[key, pair] : _queues) {
        if (--_queues_users[key].second == 0) {
#ifdef _WIN32
            CloseHandle(pair.first);

#else
            mq_close(pair.first);

#endif
            _queues_users.erase(key);
        }
    }
}

logger const *server_logger::log(std::string const &text, 
    logger::severity severity) const noexcept
{
    size_t meta_size = sizeof(bool) + sizeof(size_t) + sizeof(pid_t);
    size_t chunk_size = MESSAGE_SIZE - meta_size;
    size_t chunks_count = text.size() / chunk_size + 1;

    for (auto &[path, pair] : _queues) {
        if (pair.second.find(severity) == pair.second.end()) {
            continue;
        }
        
        char const *severity_string = severity_to_string(severity).c_str();
        char info[MESSAGE_SIZE];
        char *ptr = info;

        *reinterpret_cast<bool *>(ptr) = false;
        ptr += sizeof(bool);

        *reinterpret_cast<pid_t *>(ptr) = _process_id;
        ptr += sizeof(pid_t);

        *reinterpret_cast<size_t *>(ptr) = _session_id;
        ptr += sizeof(size_t);

        *reinterpret_cast<size_t *>(ptr) = chunks_count;
        ptr += sizeof(size_t);

        strcpy(ptr, severity_string);

#ifdef _WIN32
        DWORD bytes_written;
        WriteFile(pair.first, info, MESSAGE_SIZE, &bytes_written, nullptr);

#else
        mq_send(pair.first, info, MESSAGE_SIZE, 0);

#endif

        for (size_t i = 0; i < chunks_count; ++i) {
            char msg[MESSAGE_SIZE];
            ptr = msg;

            *reinterpret_cast<bool *>(ptr) = true;
            ptr += sizeof(bool);

            *reinterpret_cast<pid_t *>(ptr) = _process_id;
            ptr += sizeof(pid_t);

            *reinterpret_cast<size_t *>(ptr) = _session_id;
            ptr += sizeof(size_t);
            
            size_t pos = i * chunk_size;
            size_t rest = text.size() - pos;
            size_t substr_size = (rest < chunk_size) ? rest : chunk_size;

            memcpy(ptr, text.substr(pos, substr_size).c_str(), substr_size);
            *(ptr + substr_size) = '\0';

#ifdef _WIN32
            WriteFile(pair.first, msg, MESSAGE_SIZE, &bytes_written, nullptr);
#else
            mq_send(pair.first, msg, MESSAGE_SIZE, 0);

#endif
        }
    }
    
    _session_id++;
    return this;
}