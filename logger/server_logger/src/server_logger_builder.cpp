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
    _keys[stream_file_path].insert(severity);
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
    if (configuration_file_path.substr(-4) != ".ini") {
        std::logic_error not_ini_file("Configuration file must be .ini");
        throw not_ini_file;
    }

    mINI::INIFile config_file(configuration_file_path);
    mINI::INIStructure ini;
    config_file.read(ini);
    
    std::logic_error empty_value("Empty value in ini file detected\n");

    std::string streams = ini.get(configuration_path).get("streams");
    if (streams.empty()) {
        throw empty_value;
    }

    char *tokenized_streams = const_cast<char *>(streams.c_str());
    char *stream_token = strtok(tokenized_streams, ", ");

    std::vector<char *> tokens;
    while (stream_token) {
        tokens.push_back(stream_token);
        stream_token = strtok(NULL, ", ");
    }

    for (char *stream : tokens) {
        std::string severities = ini.get(configuration_path).get(stream);
        if (severities.empty()) {
            throw empty_value;
        }
        
        char *tokenized_severities = const_cast<char *>(severities.c_str());
        char *severity_token = strtok(tokenized_severities, ", ");

        while (severity_token) {
            this->add_file_stream(stream, string_to_severity(severity_token));
            severity_token = strtok(NULL, ", ");
        }
    }

    return this;
}

logger_builder *server_logger_builder::clear()
{
    for (auto &[path, pair] : _keys) {
        pair.clear();
    }
    _keys.clear();

    return this;
}

logger *server_logger_builder::build() const
{
    return new server_logger(_keys);
}