#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H

#include <cstring>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <logger_builder.h>
#include <ini.h>

#include "server_logger.h"

#ifdef __linux__
    #define CONSOLE_STREAM "/dev/tty"
#elif _WIN32
    #define CONSOLE_STREAM "CON"
#else

#endif

class server_logger_builder final:
    public logger_builder
{

public:

    server_logger_builder();

    server_logger_builder(
        server_logger_builder const &other);

    server_logger_builder &operator=(
        server_logger_builder const &other);

    server_logger_builder(
        server_logger_builder &&other) noexcept;

    server_logger_builder &operator=(
        server_logger_builder &&other) noexcept;

    ~server_logger_builder() noexcept override;

public:

    logger_builder *add_file_stream(
        std::string const &stream_file_path,
        logger::severity severity) override;

    logger_builder *add_console_stream(
        logger::severity severity) override;

    logger_builder* transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path) override;

    logger_builder *clear() override;

    [[nodiscard]] logger *build() const override;

private:
    
    std::map<std::string, std::pair<key_t, std::set<logger::severity>>> _keys;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H