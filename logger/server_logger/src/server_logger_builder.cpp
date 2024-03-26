#include "../include/server_logger_builder.h"

server_logger_builder::server_logger_builder() = default;

server_logger_builder::server_logger_builder(
    server_logger_builder const &other) = default;

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder const &other) = default;

server_logger_builder::server_logger_builder(
    server_logger_builder &&other) noexcept = default;

server_logger_builder &server_logger_builder::operator=(
    server_logger_builder &&other) noexcept = default;

server_logger_builder::~server_logger_builder() noexcept = default;

logger_builder *server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    char resolved_path[128];
    realpath(stream_file_path.c_str(), resolved_path);
    std::string resolved_path_string = resolved_path;

    _keys[resolved_path_string].insert(severity);
    return this;
}

logger_builder *server_logger_builder::add_console_stream(
    logger::severity severity)
{
    _keys[CONSOLE_STREAM].insert(severity);
    return this;
}

logger_builder* server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{
    std::runtime_error file_error("Configuration file doesn't exist\n");
    std::runtime_error configuration_error("Can't find configuration path\n");

    nlohmann::json configuration;
    std::ifstream configuration_file(configuration_file_path, std::ios::binary);
    if (configuration_file.is_open() == false) 
    {
        throw file_error;
    }

    if (configuration_file.peek() == EOF) 
    {
        throw configuration_error;
    }
    configuration_file >> configuration;
    if (configuration.find(configuration_path) == configuration.end()) 
    {
        throw file_error;
    }
    
    std::string filename;
    std::string severity_string;
    logger::severity severity_logger;

    for (auto &file : configuration[configuration_path])
    {
        filename = file[0];
        for (auto &severity : file[1])
        {
            severity_string = severity;
            severity_logger = string_to_severity(severity_string);
            _keys[filename].insert(severity_logger);
        }
    }
    
    return this;
}

logger_builder *server_logger_builder::clear()
{
    _keys.clear();

    return this;
}

logger *server_logger_builder::build() const
{
    return new server_logger(_keys);
}