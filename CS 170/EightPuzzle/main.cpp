// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>
#include <queue>
#include <vector>       // stores the expanded nodes before added to queue

// User header files
#include "Slide.h"

using namespace std;

/*********************  some typedefs ********************
 ** to make the code more similar to the given algorithm *
 *********************************************************/
typedef Slide* node;
typedef Slide* problem;
typedef queue<problem> nodes;
/*************** function pointers **********************/
typedef vector<node>(*exPnd)(node, problem);
typedef nodes(*qFunc)(nodes*, exPnd);
/*********************************************************/

void operators(Slide*);

/****************  the algorithm functions  ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes* n, exPnd); // FOR TESTING PURPOSES
vector<node> expand(node, problem);

/********* will be used for the queueing function ***********/
nodes mhat(nodes, exPnd);


int main() {
    Slide *sp = new Slide(3);
    int b[] = {8,7,1,6,0,2,5,4,3};
    
    Slide *p = new Slide(b, 3);
    
    p->print();
    cout << genSearch(p, queueFunc) << endl;

    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

bool genSearch(problem p, qFunc q) {
    Slide *goal = new Slide(p->getInputSize()); // goal state
    
    // initialize the queue with the initial state
    nodes *n = new queue<node>;
    n->push(p);
    
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        node old = n->front();
        // are you the goal state
        if (*old == *goal)
            return true; // found the solution
        *n = q(n, expand);
        cout << "now testing\n";
        n->front()->print();
        
    } while (true);
}

vector<node> expand(node current, problem p) {
    vector<node> newNodes;
    if (current->moveLeft()) {
        newNodes.push_back(current);
        current->moveRight();           // reset the tile
    }
    if (current->moveRight()) {
        newNodes.push_back(current);
        current->moveLeft();           // reset the tile
    }
    if (current->moveUp()) {
        newNodes.push_back(current);
        current->moveDown();           // reset the tile
    }
    if (current->moveDown()) {
        newNodes.push_back(current);
        current->moveUp();           // reset the tile
    }
    
    return newNodes;
}

nodes queueFunc(nodes* n, exPnd exp) {
    nodes ret;
    vector<node> newNodes = exp(n->front(), n->front());
    n->pop(); //remove old element
    // push all the new nodes that were expanded in the expand function
    for (int i = 0; i != newNodes.size(); ++i) {
        n->push(newNodes.at(i));
    }
    return ret;
}
