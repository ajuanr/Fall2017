// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>

// User header files
#include "Slide.h"

int main() {
    Slide *sp = new Slide(3);
    sp->print();
    int a[] = {3, 0, 7, 2, 8, 1, 6, 4 ,5};

    return 0;
}
