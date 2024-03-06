#include <not_implemented.h>

#include "../include/server_logger_builder.h"

server_logger_builder::server_logger_builder() = default; // ????

server_logger_builder::server_logger_builder(
    server_logger_builder const &other) 
{
    streams_severities = other.streams_severities;
}

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder const &other)
{
    return *this = server_logger_builder(other);
}

server_logger_builder::server_logger_builder(
    server_logger_builder &&other) noexcept
{
    streams_severities = std::exchange(other.streams_severities, nullptr);
}

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder &&other) noexcept
{
    std::swap(streams_severities, other.streams_severities);
    return *this;
}

server_logger_builder::~server_logger_builder() noexcept
{
    for (auto &[stream_file_path, pair] : streams_severities) {
        if (pair.first != nullptr) {
            pair.first->close();
        }
    }
    streams_severities.clear();
}

logger_builder *server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    std::ofstream *stream;
    stream->open(stream_file_path);
    
    streams_severities[stream_file_path].first = stream;
    streams_severities[stream_file_path].second.insert(severity);

    return this;
}

logger_builder *server_logger_builder::add_console_stream(
    logger::severity severity)
{
    throw not_implemented("logger_builder *server_logger_builder::add_console_stream(logger::severity severity)", "your code should be here...");
}

logger_builder* server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{
    throw not_implemented("logger_builder* server_logger_builder::transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path)", "your code should be here...");
}

logger_builder *server_logger_builder::clear()
{
    throw not_implemented("logger_builder *server_logger_builder::clear()", "your code should be here...");
}

logger *server_logger_builder::build() const
{
    server_logger _server_logger(this->streams_severities);
    return &_server_logger;
}