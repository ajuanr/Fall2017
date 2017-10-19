// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>
#include <queue>

// User header files
#include "Slide.h"

using namespace std;

/*********************  some typedefs ********************
 ** to make the code more similar to the given algorithm *
 *********************************************************/
typedef Slide* node;
typedef Slide* problem;
typedef queue<problem> nodes;
typedef node(*exPnd)(node, problem);
typedef nodes(*qFunc)(nodes,exPnd); // function pointer
/*********************************************************/

void operators(Slide*);

/****************  the algorithm functions  ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes n, exPnd) {;return n;} // FOR TESTING PURPOSES
void expand(node, exPnd);

node mhat(node, problem);

int main() {
    Slide *sp = new Slide(3);
    int b[] = {8,7,1,6,0,2,5,4,3};
    Slide *p = new Slide(b, 3);
    p->print();
    
    cout << genSearch(sp, queueFunc) << endl;

    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

bool genSearch(problem s, qFunc q) {
    Slide *goal = new Slide(s->getInputSize()); // goal state
    
    // initialize the queue with the initial state
    nodes *n = new queue<node>;
    n->push(s);
    
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        
        // are you the goal state
        if (*n->front() == *goal)
            return true; // found the solution
        
        
    } while (true);
}

node mhat(node current, problem p) {
    node next;
    if (current->moveUp())
        next = current;
    
    return next;
}
