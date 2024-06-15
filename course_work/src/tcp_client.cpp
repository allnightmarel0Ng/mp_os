#include "../include/tcp_client.h"

tcp_client::tcp_client(uint16_t port, std::string const &ip_address, size_t buffer_size):
    _buffer_size(buffer_size)
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
    {
        throw std::runtime_error("Socket creation error on clients side");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_address.c_str(), &server_address.sin_addr) <= 0)
    {
        throw std::runtime_error("Invalid address/ Address not supported");
    }

    if (connect(_socket, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address)) < 0)
    {
        throw std::runtime_error("Connection failed");
    }
}

void tcp_client::send_command(std::string const &message, std::ostream &os)
{
    send(_socket, message.c_str(), message.size(), 0);
    char buffer[_buffer_size];

    read(_socket, buffer, _buffer_size);
    std::cout << "bufsiz:" << _buffer_size << std::endl;
    std::cout << "buffer:" << buffer << std::endl;

    int answer_coefficient = std::stoi(std::string(buffer));
    if (answer_coefficient == -1)
    {
        read(_socket, buffer, _buffer_size);
        std::cout << "ERROR! " << buffer << ". Command wouldn't be interpreted" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < answer_coefficient; ++i)
        {
            read(_socket, buffer, _buffer_size);
            os << buffer;
        }
        std::cout << answer_coefficient << std::endl;
    }
}

void tcp_client::interpret_file(std::string const &filepath)
{
    std::ifstream is;
    is.open(filepath);
    if (!is.is_open())
    {
        throw std::runtime_error("Unable to open file\n");
    }
    
    std::string command;
    while (std::getline(is, command))
    {
        send_command(command);
    }

    is.close();
}

tcp_client::~tcp_client()
{
    close(_socket);
}