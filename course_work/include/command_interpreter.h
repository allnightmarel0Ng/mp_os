#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H

#include <database.h>

class command_interpreter final
{

public:

    static std::optional<std::string> interpret(std::shared_ptr<database> const &instance, std::string const &command);

    static void help_command() noexcept;

    static std::vector<std::string> tokenize_command(std::string const &command) noexcept;

    static void communicate(std::shared_ptr<database> const &instance);

    static void interpret_file(std::shared_ptr<database> const &instance, std::string const &filename);

};

#endif