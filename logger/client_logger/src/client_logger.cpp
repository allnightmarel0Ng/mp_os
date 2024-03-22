#include "../include/client_logger.h"

std::map<std::string, std::pair<std::ofstream, size_t>> client_logger::_streams_users =
    std::map<std::string, std::pair<std::ofstream, size_t>>();

client_logger::client_logger(
    std::map<std::string, std::set<logger::severity>> const &paths,
    std::string const &log_structure) 
    : _log_structure(log_structure), _log_structure_size(log_structure.size())
{   
    for (auto &[path, severities] : paths)
    {

        if (_streams_users.find(path) == _streams_users.end())
        {
            if (path != "console")
            {
                _streams_users[path].first.open(path);
                if (_streams_users[path].first.is_open() == false)
                {
                    throw std::runtime_error("Unable to open file" + path);
                }
            }

            _streams_users[path].second = 1;
        }
        else 
        {
            _streams_users[path].second++;
        }

        _streams[path] = severities;
    }
}

client_logger::client_logger(client_logger const &other) = default;

client_logger &client_logger::operator=(client_logger const &other) = default;

client_logger::client_logger(client_logger &&other) noexcept = default;

client_logger &client_logger::operator=(
    client_logger &&other) noexcept = default;

client_logger::~client_logger() noexcept
{
    for (auto &[path, pair] : _streams)
    {
        if (!--_streams_users[path].second)
        {
            _streams_users[path].first.close();
            _streams_users.erase(path);
        }
    }
}

void client_logger::format(
    std::string &to_format, std::string const &flag, 
    std::string const &replace) const noexcept
{
    auto pos = 0;
    while ((pos = to_format.find(flag, pos)) != std::string::npos)
    {
        to_format.replace(pos, flag.size(), replace);
        pos += replace.size();
    }
}

logger const *client_logger::log(std::string const &text,
    logger::severity severity) const noexcept
{
    std::cout << text << std::endl;
    std::string datetime = current_datetime_to_string();
    auto separator = datetime.find(' ');
    std::string date = datetime.substr(0, separator);
    std::string time = datetime.substr(separator);

    
    for (auto &[path, severities] : _streams)
    {
        if (severities.find(severity) == severities.end())
        {
            continue;
        }

        std::string log_message = _log_structure;
        format(log_message, "%d", date);
        format(log_message, "%t", time);
        format(log_message, "%s", severity_to_string(severity));
        format(log_message, "%m", text);

        if (_streams_users[path].first.is_open() == true) 
        {
            _streams_users[path].first << log_message;
        }
        else
        {
            std::cout << log_message;
        }
    }
    return this;
}