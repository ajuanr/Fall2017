// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>

// User header files
#include "Slide.h"

int main() {
    Slide *sp = new Slide;
    sp->print();
    Slide *rp = new Slide;
    rp->print();
    std::cout << "Testing equality\n";

    return 0;
}
