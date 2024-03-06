#include "../include/server_logger.h"

server_logger::server_logger(
    std::map<std::string, std::set<logger::severity>> _streams_severities)   
{
    for (auto &[stream_file_path, severities] : _streams_severities) {
        if (streams_users[stream_file_path].first != nullptr) {
            streams_users[stream_file_path].second++;
        }
        else {
            streams_users[stream_file_path].second = 1;
            streams_users[stream_file_path].first->open(stream_file_path);
        }

        streams_severities[stream_file_path].first = 
            streams_users[stream_file_path].first;
        
        streams_severities[stream_file_path].second = severities;
    }
}

server_logger::server_logger(server_logger const &other) 
    : streams_severities(other.streams_severities) {}

server_logger &server_logger::operator=(server_logger const &other)
{
    return *this = server_logger(other);
}

server_logger::server_logger(server_logger &&other) noexcept
{
    streams_severities = std::exchange(other.streams_severities, nullptr);
}

server_logger &server_logger::operator=(server_logger &&other) noexcept
{
    std::swap(streams_severities, other.streams_severities);
    return *this;
}

server_logger::~server_logger() noexcept
{
    for (auto &[stream_file_path, pair] : streams_severities) {
        if (!--streams_users[stream_file_path].second) {
            streams_users[stream_file_path].first->close();
        }
    }

    streams_severities.clear();
}

logger const *server_logger::log(std::string const &text, 
    logger::severity severity) const noexcept
{
    for (auto &[stream_file_path, pair] : streams_severities) {
        auto it = pair.second.find(severity);
        if (it != pair.second.end()) {
            *pair.first << text;
        }   
    }

    return this;
}