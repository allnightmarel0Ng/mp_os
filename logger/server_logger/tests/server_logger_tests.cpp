#include <iostream>
#include "../include/server_logger.h"
#include "../include/server_logger_builder.h"

#ifdef _WIN32
int test1_win()
{
    std::string pipename = "//./pipe/pipe";
    HANDLE pipe = CreateNamedPipeA(
        "//./pipe/pipe", 
        PIPE_ACCESS_DUPLEX, 
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 
        PIPE_UNLIMITED_INSTANCES, MESSAGE_SIZE, MESSAGE_SIZE, 0, nullptr);
    
    if (pipe == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Unable to create pipe");
    }

    auto *builder = new server_logger_builder;
    auto *logger = builder 
        ->add_file_stream("//./pipe/pipe", logger::severity::warning)
        ->build();
    
    if (!ConnectNamedPipe(pipe, nullptr)) {
        CloseHandle(pipe);
        throw std::runtime_error("Unable to connect\n");
    }

    char buffer[MESSAGE_SIZE];
    DWORD bytes_read;
    if (!ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytes_read, nullptr)){
        CloseHandle(pipe);
        throw std::runtime_error("Unable to receive a message\n");
    }
    
    char *ptr = buffer;
    
    bool version = *reinterpret_cast<bool *>(ptr);
    ptr += sizeof(bool);

    if (version != 0) {
        delete logger;
        CloseHandle(pipe);
        throw std::logic_error("Wrong data\n");
    }

    pid_t process_id = *reinterpret_cast<pid_t *>(ptr);
    ptr += sizeof(pid_t);

    size_t session_id = *reinterpret_cast<size_t *>(ptr);
    ptr += sizeof(size_t);

    size_t chunks_count = *reinterpret_cast<size_t *>(ptr);
    ptr += sizeof(size_t);

    char severity[16];
    strcpy(severity, ptr);

    std::cout << "Process ID: " << process_id << std::endl;
    std::cout << "Session ID: " << session_id << std::endl;
    std::cout << "Chunks Count: " << chunks_count << std::endl;
    std::cout << "Severity: " << severity << std::endl << std::endl;

    for (size_t i = 0; i < chunks_count; ++i) {
        char msg[MESSAGE_SIZE];
        DWORD bytes_read;
        if (!ReadFile(pipe, msg, sizeof(buffer) - 1, &bytes_read, nullptr)) {
            CloseHandle(pipe);
            throw std::runtime_error("Unable to receive a message\n");
        }

        ptr = msg;
        version = *reinterpret_cast<bool *>(ptr);
        if (version != 1) {
            delete logger;
            CloseHandle(pipe);
            throw std::logic_error("Wrong data\n");
        }
        ptr += sizeof(bool);

        process_id = *reinterpret_cast<pid_t *>(ptr);
        ptr += sizeof(pid_t);

        session_id = *reinterpret_cast<size_t *>(ptr);
        ptr += sizeof(size_t);

        char chunk[MESSAGE_SIZE];
        strcpy(chunk, ptr);

        std::cout << "Chunk: " << i << std::endl;
        std::cout << "Process ID: " << process_id << std::endl;
        std::cout << "Session ID: " << session_id << std::endl;
        std::cout << chunk << std::endl;
    }
    
    delete logger;
    CloseHandle(pipe);
    return 0;
}

#else
int test1_unix()
{   
    std::string filename = "/q1";
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MESSAGE_SIZE;

    mqd_t q1 = mq_open(filename.c_str(), O_CREAT | O_RDONLY, 0644, &attr);
    if (q1 == -1) {
        throw std::runtime_error("Unable to open the queue\n");
    }

    auto *builder = new server_logger_builder;
    auto *logger = builder
        ->add_file_stream(filename, logger::severity::trace)
        ->add_file_stream(filename, logger::severity::debug)
        ->build();

    delete builder;

    logger->log("111", logger::severity::trace);

    char buffer[MESSAGE_SIZE];
    unsigned int priority;
    mq_receive(q1, buffer, MESSAGE_SIZE, &priority);
    char *ptr = buffer;
    
    bool version = *reinterpret_cast<bool *>(ptr);
    ptr += sizeof(bool);

    if (version != 0) {
        delete logger;
        mq_close(q1);
        mq_unlink(filename.c_str());
        throw std::logic_error("Wrong data\n");
    }

    pid_t process_id = *reinterpret_cast<pid_t *>(ptr);
    ptr += sizeof(pid_t);

    size_t session_id = *reinterpret_cast<size_t *>(ptr);
    ptr += sizeof(size_t);

    size_t chunks_count = *reinterpret_cast<size_t *>(ptr);
    ptr += sizeof(size_t);

    char severity[16];
    strcpy(severity, ptr);

    std::cout << "Process ID: " << process_id << std::endl;
    std::cout << "Session ID: " << session_id << std::endl;
    std::cout << "Chunks Count: " << chunks_count << std::endl;
    std::cout << "Severity: " << severity << std::endl << std::endl;

    for (size_t i = 0; i < chunks_count; ++i) {
        char msg[MESSAGE_SIZE];
        mq_receive(q1, msg, MESSAGE_SIZE, NULL);
        ptr = msg;

        version = *reinterpret_cast<bool *>(ptr);
        if (version != 1) {
            delete logger;
            mq_close(q1);
            mq_unlink(filename.c_str());
            throw std::logic_error("Wrong data\n");
        }
        ptr += sizeof(bool);

        process_id = *reinterpret_cast<pid_t *>(ptr);
        ptr += sizeof(pid_t);

        session_id = *reinterpret_cast<size_t *>(ptr);
        ptr += sizeof(size_t);

        char chunk[MESSAGE_SIZE];
        strcpy(chunk, ptr);

        std::cout << "Chunk: " << i << std::endl;
        std::cout << "Process ID: " << process_id << std::endl;
        std::cout << "Session ID: " << session_id << std::endl;
        std::cout << chunk << std::endl;
    }

    delete logger;
    mq_close(q1);
    mq_unlink(filename.c_str());

    return 0;
}
#endif

int main()
{
#ifdef _WIN32
    test1_win();

#else
    if (test1_unix() == 1) {
        std::cerr << "error" << std::endl;
        return 1;
    }

#endif
    return 0;
}