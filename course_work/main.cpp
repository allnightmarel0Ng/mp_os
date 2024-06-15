#include <iostream>

#include <database.h>
#include <date_time.h>
#include <common.h>

#include <command_interpreter.h>

int main(int argc, char **argv)
{
    std::shared_ptr<database> instance = database::get_instance(8, database::mode::file);

    try
    {
        command_interpreter::communicate(instance);
    }
    catch(std::exception const &exception)
    {
        std::cout << exception.what() << std::endl;
        return -1;
    }
    

    return 0;
}
