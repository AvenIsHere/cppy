//
// Created by aven on 21/10/2026.
//

#include <iostream>
#include <utility>

#include "cppy.hpp"
using namespace cppy;

class Thing {

    int y;
    int z;
    std::string aaah;

public:

    Thing(const int y, const int z, std::string aaah) : y(y), z(z), aaah(std::move(aaah)) {}

};

int main() {
    var x = 5;
    var y = "hello";
    y = true;
    y = Thing(1, 2, "hello");

}