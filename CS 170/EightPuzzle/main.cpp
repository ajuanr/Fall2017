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
    //sp->print();
    int b[] = {3,2,8,4,5,6,7,1, 0};
    //int b[] = {1, 2, 3, 4, 5, 6, 7, 0, 8};
    Slide *p = new Slide(b, 3);
    p->print();
    p->moveRight();
    p->moveUp();
    p->print();
    cout << "Mahattan distance is: " << p->mhatDist() << endl;

    return 0;
}
