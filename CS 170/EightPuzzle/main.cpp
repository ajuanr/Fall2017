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
typedef int(*qFunc)(int);
typedef queue<problem> nodes;
/*********************************************************/

void operators(Slide*);

/**********************************************************
 ****************  the algorithm functions  ***************
 *********************************************************/
// the problem slide the queueing function
bool genSearch(problem, qFunc);
int queueFunc(int n) {cout << "passed in: " << n << endl;return n;} // FOR TESTING PURPOSES
void expand(node, int);
/*********************************************************/


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
    // the default state
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    Slide *goal = new Slide(a, 3); // CHANGE THE USE OF THE CONSTANT HERE
    
    // initialize the stack
    nodes *n = new queue<Slide*>;
    n->push(s);
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        
        // are you the goal state
        if (*n->front() == *goal)
            return true;
    } while (true);
    
}
