#include "../include/automatic_logger.h"

automatic_logger::automatic_logger(logger::severity severity, std::string const &method_name, std::string const &type, logger *_logger)
    : _severity(severity), _method_name(method_name), _type(type), _logger(_logger)
{
    log_with_guard("START: " + _type + ": " + _method_name, _severity);
}

automatic_logger::~automatic_logger()
{
    log_with_guard("END: " + _type + ": " + _method_name, _severity);
}

automatic_logger::automatic_logger(automatic_logger const &other) 
    : _severity(other._severity), 
    _method_name(other._method_name), 
    _type(other._type), 
    _logger(other._logger) {}

automatic_logger &automatic_logger::operator=(automatic_logger const &other)
{
    if (this == &other)
    {
        return *this;
    }
    
    _severity = other._severity;
    _method_name = other._method_name;
    _type = other._type;
    _logger = other._logger;

    return *this;
}

automatic_logger::automatic_logger(automatic_logger &&other) noexcept 
    : _severity(other._severity), 
    _method_name(std::move(other._method_name)), 
    _type(std::move(other._type)),
    _logger(std::move(other._logger)) {}

automatic_logger &automatic_logger::operator=(automatic_logger &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    _severity = other._severity;
    _method_name = std::move(other._method_name);
    _type = std::move(other._type);
    _logger = std::move(other._logger);

    return *this;
}

inline logger *automatic_logger::get_logger() const
{
    return _logger;
}