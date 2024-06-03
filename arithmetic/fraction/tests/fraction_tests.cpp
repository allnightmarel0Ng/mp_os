#include <iostream>

#include <fraction.h>

int main()
{
    fraction f1(big_integer("1"), big_integer("25")), f2(big_integer("1"), big_integer("2"));
    std::cin >> f2;
    std::cout << f2.log2(fraction(big_integer("1"), big_integer("100"))) << std::endl;
    return 0;
}