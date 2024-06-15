#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <ctime>
#include <chrono>
#include <iomanip>
#include <regex>

class date_time final
{

private:

    std::chrono::system_clock::time_point _time_point;

public:

    explicit date_time();

    explicit date_time(std::string const &iso8601);

    explicit date_time(int64_t nanoseconds);

public:

    operator int64_t() const;

    operator std::string() const;

public:

    static int comparer(date_time const &one, date_time const &another);

};

#endif