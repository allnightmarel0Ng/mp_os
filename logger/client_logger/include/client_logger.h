#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H

#include <logger.h>
#include "client_logger_builder.h"

class client_logger final:
    public logger
{

    friend class client_logger_builder;

public:

    client_logger(
        client_logger const &other);

    client_logger &operator=(
        client_logger const &other);

    client_logger(
        client_logger &&other) noexcept;

    client_logger &operator=(
        client_logger &&other) noexcept;

    ~client_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

private:

    client_logger(
        std::map<std::string, std::set<logger::severity>> const &paths,
        std::string const &log_structure);
    
    void format(
        std::string &to_format, std::string const &flag, 
        std::string const &replace_with) const noexcept;

private:
    
    std::map<std::string, std::pair<std::ostream *, std::set<logger::severity>>> _streams;

    static std::map<std::string, std::pair<std::ostream *, size_t>> _streams_users;

    std::string _log_structure;

    size_t _log_structure_size;
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H