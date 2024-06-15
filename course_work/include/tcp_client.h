#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <command_interpreter.h>

class tcp_client final
{

public:

    explicit tcp_client(uint16_t port, std::string const &ip_address, size_t buffer_size = 1024);

public:

    ~tcp_client();

    tcp_client(tcp_client const &) = delete;

    tcp_client(tcp_client &&) = delete;

public:

    void send_command(std::string const &command, std::ostream &os = std::cout);

public:

    void interpret_file(std::string const &filepath);

private:

    int _socket;

    sockaddr_in server_address;

    size_t _buffer_size;

};

#endif