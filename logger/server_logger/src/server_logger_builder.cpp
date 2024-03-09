#include "../include/server_logger_builder.h"

server_logger_builder::server_logger_builder() = default;

server_logger_builder::server_logger_builder(
    server_logger_builder const &other) 
    : _keys(other._keys) {}

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder const &other)
{
    return *this = server_logger_builder(other);
}

server_logger_builder::server_logger_builder(
    server_logger_builder &&other) noexcept
{
    _keys = std::exchange(other._keys, nullptr);
}

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder &&other) noexcept
{
    std::swap(_keys, other._keys);
    return *this;
}

server_logger_builder::~server_logger_builder() noexcept = default;

logger_builder *server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    _keys[stream_file_path].first = ftok(stream_file_path.c_str(), 1);
    _keys[stream_file_path].second.insert(severity);
    return this;
}

logger_builder *server_logger_builder::add_console_stream(
    logger::severity severity)
{
    _keys[CONSOLE_STREAM].first = ftok(CONSOLE_STREAM, 1);
    _keys[CONSOLE_STREAM].second.insert(severity);
    return this;
}

logger_builder* server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{
    
}

logger_builder *server_logger_builder::clear()
{
    for (auto &[stream_file_path, pair] : _keys) {
        pair.second.clear();
    }
    _keys.clear();

    return this;
}

logger *server_logger_builder::build() const
{
    server_logger _server_logger(this->_keys);
    return &_server_logger;
}