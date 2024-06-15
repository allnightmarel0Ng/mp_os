#include "../include/tcp_storage_server.h"

tcp_storage_server::tcp_storage_server(uint16_t port, size_t t_for_b_tree, database::mode mode, allocator *allocator_for_database, logger *logger_for_database, size_t buffer_size, uint16_t backlog):
    _instance(database::get_instance(t_for_b_tree, mode, database::container_variant::b_tree, allocator_for_database, logger_for_database)),
    _buffer_size(buffer_size)
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == 0)
    {
        throw std::runtime_error("socket error");
    }

    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_opt, sizeof(_opt)))
    {
        throw std::runtime_error("setsockopt failure");
    }

    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(port);

    if (bind(_server_fd, reinterpret_cast<sockaddr *>(&_address), sizeof(_address)) < 0) 
    {
        throw std::runtime_error("bind failure");
    }

    if (listen(_server_fd, backlog) < 0) 
    {
        throw std::runtime_error("listen failure");
    }
}

void tcp_storage_server::run()
{
    socklen_t addrlen = sizeof(_address);
    _accept_fd = accept(_server_fd, reinterpret_cast<sockaddr *>(&_address), &addrlen);
    if (_accept_fd < 0)
    {
        throw std::runtime_error("acceptance error");
    }

    while (true)
    {
        char buffer[_buffer_size];
        read(_accept_fd, buffer, _buffer_size);
        std::cout << buffer << std::endl;
        std::cout << _buffer_size << std::endl;

        std::string command(buffer);
        if (command == "exit")
        {
            return;
        }
        
        std::optional<std::string> interpreted;
        try
        {
            interpreted = command_interpreter::interpret(_instance.get(), command);
        }
        catch (std::logic_error const &exception)
        {
            auto what = exception.what();
            send(_accept_fd, "-1", 2, 0);
            send(_accept_fd, what, strlen(what), 0);
        }

        if (interpreted.has_value())
        {
            size_t chunks_count = interpreted.value().size() / _buffer_size + 1;
            auto string_chunks_count = std::to_string(chunks_count);
            std::cout << "chunks count: " << string_chunks_count << std::endl;
            send(_accept_fd, string_chunks_count.c_str(), string_chunks_count.size(), 0);
            size_t offset = 0;
            sleep(1);
            for (size_t i = 0; i < chunks_count; ++i)
            {
                size_t chunk_size = std::min(interpreted.value().size() - offset, _buffer_size);
                auto chunk = interpreted.value().substr(offset, chunk_size);
                offset += chunk_size;
                send(_accept_fd, chunk.c_str(), chunk_size, 0);
            }
        }
        else
        {
            send(_accept_fd, "0", 1, 0);
        }

    }
}

tcp_storage_server::~tcp_storage_server()
{
    close(_accept_fd);
    close(_server_fd);
}