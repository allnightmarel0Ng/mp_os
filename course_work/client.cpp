#include <tcp_client.h>

int main()
{
    tcp_client client(34543, "127.0.0.1");

    std::string command;
    while (std::getline(std::cin, command))
    {
        auto tokenized_command = command_interpreter::tokenize_command(command);
        size_t tokenized_command_size = tokenized_command.size();

        if (tokenized_command_size == 0)
        {
            std::cout << "ERROR: empty command\n" << std::endl;
            return -1;
        }

        auto it = tokenized_command.cbegin();
        if (*it == "exit" && tokenized_command_size == 1)
        {
            client.send_command(*it);
            break;
        }

        if (*it == "input" && tokenized_command_size == 2)
        {
            try
            {
                client.interpret_file(*++it);
            } 
            catch (std::runtime_error const &exception)
            {
                std::cout << exception.what() << std::endl;
                return -1;
            }
        }
        else if (*it == "help" && tokenized_command_size == 1)
        {
            command_interpreter::help_command();
        }
        else
        {
            client.send_command(command.c_str());
        }
    }

    return 0;
}