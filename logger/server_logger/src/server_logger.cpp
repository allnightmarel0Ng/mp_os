#include "../include/server_logger.h"

server_logger::server_logger(std::map<std::string, 
    std::pair<key_t, std::set<logger::severity>>> const keys)   
{   
    std::runtime_error queue_error("Unable to open queue\n");
    
    for (auto &[stream_file_path, pair] : keys) {
        if (_queues_users.find(stream_file_path) == _queues_users.end()) {
            int queue = msgget(pair.first, 0666 | IPC_CREAT);
            if (queue == -1) {
                throw queue_error;
            }
            
            _queues_users[stream_file_path].first = &queue;
            _queues_users[stream_file_path].second = 1;

            _queues[stream_file_path].first = &queue;
        }
        else {
            _queues_users[stream_file_path].second++;
            _queues[stream_file_path].first =
                _queues_users[stream_file_path].first;
        } 

        _queues[stream_file_path].second = pair.second;
    }
}

server_logger::server_logger(server_logger const &other) = default;

server_logger &server_logger::operator=(server_logger const &other) = default;

server_logger::server_logger(server_logger &&other) noexcept = default;

server_logger &server_logger::operator=(
    server_logger &&other) noexcept = default;

server_logger::~server_logger() noexcept
{
    for (auto &[stream_file_path, pair] : _queues) {
        if (--_queues_users[stream_file_path].second == 0) {
            msgctl(*_queues_users[stream_file_path].first, IPC_RMID, NULL);
            _queues_users.erase(stream_file_path);
        }
    }
}

logger const *server_logger::log(std::string const &text, 
    logger::severity severity) const noexcept
{
    size_t chunks_count = sizeof(text) / 1024 + 1;
    msgbuf msgbuf_array[chunks_count];

    for (size_t i = 0; i < chunks_count; ++i) {
        msgbuf_array[i].mtype = 1;
        
        size_t pos = i * 1024;
        size_t leftover = text.size() - pos;
        size_t substring_size = (leftover < 1024) ? leftover : 1024;
        
        strcpy(msgbuf_array[i].mtext, text.substr(pos, substring_size).c_str());
    }

    for (auto &[stream_file_path, pair] : _queues) {
        if (pair.second.find(severity) == pair.second.end()) {
            continue;
        }

        msgbuf_info _msgbuf_info = {
            .mtype = 1, 
            .minfo = {chunks_count, severity}};
        msgsnd(*pair.first, &_msgbuf_info, sizeof(_msgbuf_info), 0);
        
        for (size_t i = 0; i < chunks_count; ++i) {
            msgsnd(*pair.first, msgbuf_array + i, sizeof(msgbuf_array[i]), 0);
        }
    }
    return this;
}