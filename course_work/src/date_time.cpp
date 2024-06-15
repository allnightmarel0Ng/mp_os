#include "../include/date_time.h"

date_time::date_time():
    _time_point(std::chrono::system_clock::now())
{

}

date_time::date_time(std::string const &iso8601)
{
    std::tm tm;
    std::istringstream in(iso8601);
    
    in >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (in.fail()) 
    {
        throw std::runtime_error("Failed to parse ISO 8601 string");
    }

    _time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

date_time::date_time(int64_t nanoseconds):
    _time_point(std::chrono::system_clock::time_point(std::chrono::nanoseconds(nanoseconds)))
{

}

date_time::operator int64_t() const
{
    auto duration = _time_point.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return nanoseconds.count();
}

date_time::operator std::string() const
{
    auto time_t_time_point = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::nanoseconds>(_time_point));
    auto tm_time_point = std::localtime(&time_t_time_point);

    std::ostringstream oss;
    oss << std::put_time(tm_time_point, "%Y-%m-%dT%H:%M:%SZ");

    auto duration = _time_point.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration % std::chrono::seconds(1));

    if (nanoseconds.count() > 0)
    {
        oss << '.' << std::setfill('0') << std::setw(9) << nanoseconds.count();
    }

    return oss.str();
}

int date_time::comparer(date_time const &one, date_time const &another)
{
    if (static_cast<int64_t>(one) == static_cast<int64_t>(another))
    {
        return 0;
    }
    return static_cast<int64_t>(one) > static_cast<int64_t>(another) ? 1 : -1;
}