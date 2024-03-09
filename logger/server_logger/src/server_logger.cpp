#include "../include/server_logger.h"

server_logger::server_logger(
    std::map<std::string, 
    std::pair<key_t, std::set<logger::severity>>> const keys)   
{   
    std::runtime_error queue_error("Unable top open queue\n");
    
    for (auto &[stream_file_path, pair] : keys) {
        int queue = msgget(pair.first, 0666 | IPC_CREAT);
        if (queue == -1) {
            throw queue_error;
        }
        
        _queues[stream_file_path].first = queue;
        _queues[stream_file_path].second = pair.second;

        if (_queues_users.find(stream_file_path) == _queues_users.end()) {
            _queues_users[stream_file_path].first = &queue;
            _queues_users[stream_file_path].second = 1;
        }
        else {
            _queues_users[stream_file_path].second++;
        } 
    }
}

server_logger::server_logger(server_logger const &other) 
    : _queues(other._queues) {}

server_logger &server_logger::operator=(server_logger const &other)
{
    return *this = server_logger(other);
}

server_logger::server_logger(server_logger &&other) noexcept
{
    _queues = std::exchange(other._queues, nullptr);
}

server_logger &server_logger::operator=(server_logger &&other) noexcept
{
    std::swap(_queues, other._queues);
    return *this;
}

server_logger::~server_logger() noexcept
{
    for (auto &[stream_file_path, pair] : _queues) {
        if (!--_queues_users[stream_file_path].second) {
            msgctl(*_queues_users[stream_file_path].first, IPC_RMID, NULL);
        }
    }
}

logger const *server_logger::log(std::string const &text, 
    logger::severity severity) const noexcept
{
    size_t chunks_count = sizeof(text) / 1024 + 1;
    msgbuf msgbuf_array[chunks_count];

    for (size_t i = 0; i < 1024; i++) {
        strcpy(msgbuf_array[i].mtext, text.substr(i * 1024, 1024).c_str());
        msgbuf_array[i].mtype = 1;
    }

    for (auto &[stream_file_path, pair] : _queues) {
        if (pair.second.find(severity) != pair.second.end()) {
            for (size_t i = 0; i < chunks_count; i++) {
                msgsnd(pair.first, msgbuf_array + i, 
                    sizeof(msgbuf_array + i), 0);
            }
        }
    }
    return this;
}