// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>
#include <stack>

// User header files
#include "Slide.h"

using namespace std;

// the problem slide the queueing function
bool genSearch(Slide*, int (*)(int));
int queueFunc(int n) { cout << "n is: " << n << endl;return n;} // FOR TESTING PURPOSES

int main() {
    Slide *sp = new Slide(3);
    int b[] = {8,7,1,6,0,2,5,4,3};
    Slide *p = new Slide(b, 3);
    p->print();
    
    cout << genSearch(sp, queueFunc) << endl;

    return 0;
}

bool genSearch(Slide* s, int(*q)(int n)) {
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    Slide *goal = new Slide(a, 3); // CHANGE THE USE OF THE CONSTANT HERE
    // initialize the stack
    stack<Slide*> *nodes = new stack<Slide*>;
    nodes->push(s);
    do {
        // are you the goal state
        q(3);
        if (*nodes->top() == *goal)
            return true;
    } while (true);
    
}
