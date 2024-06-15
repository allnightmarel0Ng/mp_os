#include "../include/command_interpreter.h"

std::optional<std::string> command_interpreter::interpret(std::shared_ptr<database> const &instance, std::string const &command)
{
    auto get_tdata_from_index = [](std::string const &index_name, std::string const &field)
        {
            tkey key;
            tvalue value;

            if (index_name == "main_key")
            {
                key.id = std::stoul(field);
            }
            else if (index_name == "name")
            {
                value.name = field;
            }
            else if (index_name == "surname")
            {
                value.surname = field;
            }
            else if (index_name == "birthday")
            {
                value.birthday = date_time(field);
            }
            else if (index_name == "marks")
            {
                value.marks = std::move(std::vector<unsigned char>());
                for (auto const &mark: field)
                {
                    value.marks.push_back(mark);
                }
            }

            return tdata(key, value);
        };

    auto insert_and_update_processing = [](std::vector<std::string> const &tokens)
        {
            auto it = tokens.cbegin();
            auto pool_name = *++it;
            auto scheme_name = *++it;
            auto collection_name = *++it;

            tkey key;
            key.id = std::stoul(*++it);

            tvalue value;
            value.name = *++it;
            value.surname = *++it;
            value.birthday = date_time(*++it);
            value.marks = std::move(std::vector<unsigned char>());

            auto marks = *++it;
            for (auto const &mark: marks)
            {
                value.marks.push_back(static_cast<unsigned char>(mark));
            }
            
            return std::tuple<std::string, std::string, std::string, tkey, tvalue>(pool_name, scheme_name, collection_name, key, value);
        };
    
    auto obtain_and_dispose_processing = [get_tdata_from_index](std::vector<std::string> const &tokens)
        {
            auto it = tokens.cbegin();
            
            auto pool_name = *++it;
            auto scheme_name = *++it;
            auto collection_name = *++it;
            auto index_name = *++it;
            auto field = *++it;

            return std::tuple<std::string, std::string, std::string, tdata, std::string>(pool_name, scheme_name, collection_name, get_tdata_from_index(index_name, field), index_name);
        };

    auto print_results = [](std::vector<tdata> const &obtained)
        {
            std::ostringstream oss;
            oss << "| id |    name    |    surname    |    birthday    |    marks    |" << std::endl;
            for (auto const &data: obtained)
            {
                oss << "| " << data.key.id << " | " << data.value.name << " | " << data.value.surname << " | " << static_cast<std::string>(data.value.birthday) << " | ";
                for (auto const &mark: data.value.marks)
                {
                    oss << mark;
                }
                oss << " |" << std::endl;
            }
            return oss.str();
        };
    
    std::string result;

    auto command_tokens = tokenize_command(command);
    size_t tokens_count = command_tokens.size();
    if (!tokens_count)
    {
        throw std::logic_error("No command provided");
    }

    auto it = command_tokens.cbegin();

    if (*it == "add_pool" && tokens_count == 3)
    {
        instance->add_pool(*++it, database::container_variant::b_tree, std::stoul(command_tokens.back()));
    }
    else if (*it == "add_scheme" && tokens_count == 4)
    {
        auto pool_name = *++it;
        auto scheme_name = *++it;
        auto t_for_b_trees = std::stoul(*++it);

        instance->add_scheme(pool_name, scheme_name, database::container_variant::b_tree, t_for_b_trees);
    }
    else if (*it == "add_collection" && tokens_count == 5)
    {
        auto pool_name = *++it;
        auto scheme_name = *++it;
        auto collection_name = *++it;
        auto t_for_b_trees = std::stoul(*++it);

        instance->add_collection(pool_name, scheme_name, collection_name, database::container_variant::b_tree, t_for_b_trees);
    }
    else if (*it == "dispose_pool" && tokens_count == 2)
    {
        instance->dispose_pool(*++it);
    }
    else if (*it == "dispose_scheme" && tokens_count == 3)
    {
        auto pool_name = *++it;
        auto scheme_name = *++it;
        instance->dispose_scheme(pool_name, scheme_name);
    }
    else if (*it == "dispose_collection" && tokens_count == 4)
    {
        auto pool_name = *++it;
        auto scheme_name = *++it;
        auto collection_name = *++it;
        
        instance->dispose_collection(pool_name, scheme_name, collection_name);
    }
    else if (*it == "insert_data" && tokens_count == 9)
    {
        auto processed_command = insert_and_update_processing(command_tokens);

        instance->insert(std::get<0>(processed_command), std::get<1>(processed_command), std::get<2>(processed_command), std::get<3>(processed_command), std::get<4>(processed_command));
    }
    else if (*it == "update_data" && tokens_count == 9)
    {
        auto processed_command = insert_and_update_processing(command_tokens);

        instance->update(std::get<0>(processed_command), std::get<1>(processed_command), std::get<2>(processed_command), std::get<3>(processed_command), std::get<4>(processed_command));
    }
    else if (*it == "obtain_data" && tokens_count == 7)
    {
        auto processed_command = obtain_and_dispose_processing(command_tokens);
        auto target_time = command_tokens.back() == "now" ? date_time() : date_time(command_tokens.back());

        auto obtained = instance->obtain(std::get<0>(processed_command), std::get<1>(processed_command), std::get<2>(processed_command), std::get<3>(processed_command), std::get<4>(processed_command), target_time);
        
        result = print_results(obtained);
    }
    else if (*it == "obtain_between_data" && tokens_count == 10)
    {
        auto pool_name = *++it;
        auto scheme_name = *++it;
        auto collection_name = *++it;

        auto index_name = *++it;

        auto lower_bound = get_tdata_from_index(index_name, *++it);
        auto upper_bound = get_tdata_from_index(index_name, *++it);

        auto target_time = *++it == "now" ? date_time() : date_time(*it);
        
        bool lower_bound_inclusive = (*++it == "true" || *it == "1") 
            ? true
            : (*it == "false" || *it == "0")
                ? false
                : throw std::logic_error("Can't parse" + *it + "to bool");
        bool upper_bound_inclusive = (*++it == "true" || *it == "1") 
            ? true
            : (*it == "false" || *it == "0")
                ? false
                : throw std::logic_error("Can't parse" + *it + "to bool");

        auto obtained = instance->obtain_between(pool_name, scheme_name, collection_name, lower_bound, upper_bound, index_name, target_time, lower_bound_inclusive, upper_bound_inclusive);

        result = print_results(obtained);
    }
    else if (*it == "dispose_data" && tokens_count == 6)
    {
        auto processed_command = obtain_and_dispose_processing(command_tokens);
        instance->dispose(std::get<0>(processed_command), std::get<1>(processed_command), std::get<2>(processed_command), std::get<3>(processed_command), std::get<4>(processed_command));
    }
    else
    {
        throw std::logic_error("Unknown command or wrong amount of arguments");
    }
    
    return result.empty() ? std::optional<std::string>() : std::optional<std::string>(result);
}

void command_interpreter::help_command() noexcept
{
    std::cout << "Put off angle brackets in commands. In parentheses requirements for commands are specified." << std::endl;
    std::cout << "One command at command" << std::endl;
    
    std::cout << "Container actions:" << std::endl;
    std::cout << "add_pool <pool_name> <t_for_b_tree (positive integer number)> - add new pool to database" << std::endl;
    std::cout << "add_scheme <pool_name> <scheme_name> <t_for_b_tree (positive integer number)> - add new scheme to existent pool" << std::endl;
    std::cout << "add_collection <pool_name> <scheme_name> <collection_name> <t_for_b_tree (positive integer number)> - add new collection to already existent pool and existent scheme" << std::endl;
    std::cout << "dispose_pool <pool_name> - delete pool from database" << std::endl;
    std::cout << "dispose_scheme <pool_name> <scheme_name> - delete scheme from pool" << std::endl;
    std::cout << "dispose_collection <pool_name> <scheme_name> <collection_name> - dispose collection from scheme\n" << std::endl;
    
    std::cout << "Data actions:" << std::endl;
    std::cout << "insert_data <pool_name> <scheme_name> <collection_name> <id> <name> <surname> <birthday (iso8601)> <marks (no spaces, just numbers)> - add new data to collection" << std::endl;
    std::cout << "update_data <pool_name> <scheme_name> <collection_name> <id> <name> <surname> <birthday (iso8601)> <marks (no spaces, just numbers)> - update data in some collection, <id> must to exist in this collection" << std::endl;
    std::cout << "obtain_data <pool_name> <scheme_name> <collection_name> <index (main_key, name, surname, birthday, marks)> <data_to_find (id if main_key in index, name if name specified in index, etc)> <datetime_target (iso8601, or <now> for current system time)> - obtains data from collection and prints it to ouput stream" << std::endl;
    std::cout << "obtain_between_data <pool_name> <scheme_name> <collection_name> <index (main_key, name, surname, birthday, marks)> <lower_bound (id if main_key in index, name if name specified in index, etc)> <upper_bound> <date_time_target (iso8601, or <now> for current system time)> <lower_bound_inclusive_flag (0, 1 or false, true)> <upper_bound_inclusive_flag (0, 1 or false, true)>" << std::endl;
    std::cout << "dispose_data <pool_name> <scheme_name> <collection_name> <index (main_key, name, surname, birthday, marks)> <data_to_delete (id if main_key in index, name if name specified in index, etc)>\n" << std::endl;
    std::cout << "Common actions (this commands are available ONLY in standard input, using them in file input will lead to exception): " << std::endl;
    std::cout << "input <filepath> - change input stream" << std::endl;
    std::cout << "exit - to exit the program\n" << std::endl;
}

std::vector<std::string> command_interpreter::tokenize_command(std::string const &command) noexcept
{
    std::vector<std::string> result;
    std::istringstream iss(command);
    std::string token;

    while (std::getline(iss, token, ' '))
    {
        if (!token.empty())
        {
            result.push_back(token);
        }
    }

    return result;
}

void command_interpreter::communicate(std::shared_ptr<database> const &instance)
{
    std::string command;
    std::cout << "Type <help> - for documentation and <exit> - to exit the program\n>> ";
    while (std::getline(std::cin, command))
    {
        auto tokenized_command = command_interpreter::tokenize_command(command);
        size_t tokenized_command_size = tokenized_command.size();
        auto it = tokenized_command.cbegin();
        
        if (*it == "exit" && tokenized_command_size == 1)
        {
            break;
        }
        if (*it == "help" && tokenized_command_size == 1)
        {
            command_interpreter::help_command();
        }
        else if (*it == "input" && tokenized_command_size == 2)
        {
            interpret_file(instance, *++it);
        }
        else
        {
            auto interpreted = command_interpreter::interpret(instance, command);
            
            if (interpreted.has_value())
            {
                std::cout << interpreted.value() << std::endl;
            }
        }
        std::cout << ">> ";
    }
}

void command_interpreter::interpret_file(std::shared_ptr<database> const &instance, std::string const &filename)
{
    std::ifstream is;
    is.open(filename);
    if (!is.is_open())
    {
        throw std::runtime_error("Unable to open file\n");
    }

    std::string command;
    while (std::getline(is, command))
    {
        command_interpreter::interpret(instance, command);
    }
}