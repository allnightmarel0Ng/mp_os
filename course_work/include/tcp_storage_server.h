#ifndef TCP_STORAGE_SERVER_H
#define TCP_STORAGE_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <command_interpreter.h>
#include <database.h>


class tcp_storage_server final
{

public:

    explicit tcp_storage_server(uint16_t port, size_t t_for_b_tree = 8, database::mode mode = database::mode::in_memory, allocator *allocator_for_database = nullptr, logger *logger_for_database = nullptr, size_t buffer_size = 1024, uint16_t backlog = 5);

public:

    ~tcp_storage_server();

    tcp_storage_server(tcp_storage_server const &) = delete;

    tcp_storage_server(tcp_storage_server &&) = delete;

public:

    void run();

private:

    int _server_fd;

    int _accept_fd;

    struct sockaddr_in _address;

    int _opt = 1;

    std::shared_ptr<database> _instance; 

    size_t _buffer_size;

};

#endif