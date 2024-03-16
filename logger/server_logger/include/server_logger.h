#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include <cstring>

#include "../../logger/include/logger.h"
#include "server_logger_builder.h"

#define MESSAGE_SIZE 1024

class server_logger final:
    public logger
{

    friend class server_logger_builder;

public:

    server_logger(server_logger const &other);

    server_logger &operator=(server_logger const &other);

    server_logger(server_logger &&other) noexcept;

    server_logger &operator=(server_logger &&other) noexcept;

    ~server_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(const std::string &message,
        logger::severity severity) const noexcept override;

private:
    
    server_logger(std::map<std::string, std::set<logger::severity>> const keys);

#ifdef _WIN32
    std::map<std::string, std::pair<HANDLE, std::set<logger::severity>>> _queues;

    static std::map<std::string, std::pair<HANDLE, size_t>> _queues_users;

    DWORD _process_id;
#else
    std::map<std::string, std::pair<mqd_t, std::set<logger::severity>>> _queues;

    static std::map<std::string, std::pair<mqd_t, size_t>> _queues_users;

    pid_t _process_id;
#endif

    size_t mutable _session_id;
    
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H