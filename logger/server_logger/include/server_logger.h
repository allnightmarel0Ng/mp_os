#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include <logger.h>
#include "server_logger_builder.h"

class server_logger final:
    public logger
{

    friend class server_logger_builder;

public:


    server_logger(
        server_logger const &other);

    server_logger &operator=(
        server_logger const &other);

    server_logger(
        server_logger &&other) noexcept;

    server_logger &operator=(
        server_logger &&other) noexcept;

    ~server_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

private:
    
    server_logger(std::map<std::string, 
        std::set<logger::severity>> const _streams_severities);

    std::map<std::string, std::pair<std::ofstream *, 
        std::set<logger::severity>>> streams_severities;

    static std::map<std::string, 
        std::pair<std::ofstream *, int>> streams_users;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H