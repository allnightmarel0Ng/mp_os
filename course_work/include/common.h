#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <optional>

#include <date_time.h>

int string_comparer(std::string const &one, std::string const &another);

struct tkey final
{
    unsigned int id;   
};

struct tvalue final
{
    std::string name;
    
    std::string surname;
    
    date_time birthday;

    std::vector<unsigned char> marks;

public:

    tvalue() = default;

public:

    ~tvalue() = default;

    tvalue(tvalue const &other) = default;

    tvalue &operator=(tvalue const &other) = default;

    tvalue(tvalue &&other) noexcept = default;

    tvalue &operator=(tvalue &&other) noexcept = default;

};

class tdata final
{

public:

    tkey key;

    tvalue value;

public:

    tdata() = default;
    
    tdata(tkey const &key, tvalue const &value);

    tdata(tkey const &key, tvalue &&value);

public:

    ~tdata() = default;

    tdata(tdata const &other) = default;

    tdata &operator=(tdata const &other) = default;

    tdata(tdata &&other) noexcept = default;

    tdata &operator=(tdata &&other) noexcept = default;

public:

    static int key_comparer(tdata const &one, tdata const &another);

    static int name_comparer(tdata const &one, tdata const &another);

    static int surname_comparer(tdata const &one, tdata const &another);

    static int birthday_comparer(tdata const &one, tdata const &another);

    static int marks_comparer(tdata const &one, tdata const &another);

};

#endif