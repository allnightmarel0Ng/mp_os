#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H

#ifdef _WIN32
    #define CONSOLE_STREAM "CON"
    #include <conio.h>
    #include <windows.h>
#else
    #define CONSOLE_STREAM "/dev/tty"
    #include <unistd.h>
    #include <mqueue.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#include <cstring>

#include "../../../common/mini/ini.h"
#include "../../logger/include/logger_builder.h"
#include "server_logger.h"


class server_logger_builder final:
    public logger_builder
{

public:

    server_logger_builder();

    server_logger_builder(server_logger_builder const &other);

    server_logger_builder &operator=(server_logger_builder const &other);

    server_logger_builder(server_logger_builder &&other) noexcept;

    server_logger_builder &operator=(server_logger_builder &&other) noexcept;

    ~server_logger_builder() noexcept override;

public:

    logger_builder *add_file_stream(std::string const &stream_file_path,
        logger::severity severity) override;

    logger_builder *add_console_stream(logger::severity severity) override;

    logger_builder* transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path) override;

    logger_builder *clear() override;

    [[nodiscard]] logger *build() const override;

private:
    
    std::map<std::string, std::set<logger::severity>> _keys;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H