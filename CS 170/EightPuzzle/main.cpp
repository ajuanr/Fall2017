// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>

// User header files
#include "Slide.h"

using namespace std;

int main() {
    Slide *sp = new Slide(3);
    sp->print();
    int a[] = {3, 0, 7, 2, 8, 1, 6, 4 ,5};
    sp->moveLeft();
    sp->moveUp();
    sp->moveRight();
    sp->moveUp();
    sp->print();
    cout << "Number of tiles miplaced: " << sp->misTiles() << endl;

    return 0;
}
