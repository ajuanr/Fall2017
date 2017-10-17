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
   std::cout << "Move left retsult: " << sp->moveLeft() << std::endl;
    sp->print();
//    std::cout << "Move left retsult: " << sp->moveLeft() << std::endl;
//    std::cout << "Move left retsult: " << sp->moveLeft() << std::endl;
        std::cout << "Move left retsult: " << sp->moveRight() << std::endl;
    sp->print();
    return 0;
}
