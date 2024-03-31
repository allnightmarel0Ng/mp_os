#ifndef AUTOMATIC_LOGGER_H
#define AUTOMATIC_LOGGER_H

#include "logger_guardant.h"
#include "logger.h"

class automatic_logger final:
    public logger_guardant
{

public:
    
    explicit automatic_logger(
        logger::severity severity, std::string const &method_name, std::string const &type_name, logger *_logger);

    virtual ~automatic_logger();

    automatic_logger(automatic_logger const &other);
    
    automatic_logger &operator=(automatic_logger const &other);

    automatic_logger(automatic_logger &&other) noexcept;

    automatic_logger &operator=(automatic_logger &&other) noexcept;

public:

    inline logger *get_logger() const override;

private:
    
    logger::severity _severity;

    std::string _method_name;

    std::string _type;

    logger *_logger;

};

#endif