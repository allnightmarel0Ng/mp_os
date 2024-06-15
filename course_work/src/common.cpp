#include "../include/common.h"

int string_comparer(std::string const &one, std::string const &another)
{
    if (one == another)
    {
        return 0;
    }

    return one > another ? 1 : -1;
}

tdata::tdata(tkey const &key, tvalue const &value):
    key(key), value(value)
{
    
}

tdata::tdata(tkey const &key, tvalue &&value):
    key(key), value(std::move(value))
{

}

int tdata::key_comparer(tdata const &one, tdata const &another)
{
    return static_cast<int>(one.key.id) - static_cast<int>(another.key.id);
}

int tdata::name_comparer(tdata const &one, tdata const &another)
{
    return string_comparer(one.value.name, another.value.name);
}

int tdata::surname_comparer(tdata const &one, tdata const &another)
{
    return string_comparer(one.value.surname, another.value.surname);
}

int tdata::birthday_comparer(tdata const &one, tdata const &another)
{
    return date_time::comparer(one.value.birthday, another.value.birthday);
}

int tdata::marks_comparer(tdata const &one, tdata const &another)
{
    std::string one_string, another_string;
    
    for (auto const &mark: one.value.marks)
    {
        one_string.push_back(static_cast<char>(mark));
    }

    for (auto const &mark: another.value.marks)
    {
        another_string.push_back(static_cast<char>(mark));
    }

    return string_comparer(one_string, another_string);
}