// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>
#include <queue>        // FIFO
#include <vector>       // stores the expanded nodes before added to queue

// User header files
#include "Slide.h"

using namespace std;

/*********************  some typedefs ********************
 ** to make the code more similar to the given algorithm *
 *********************************************************/
typedef Slide* node;
typedef Slide* problem;
typedef priority_queue<problem> nodes;

/*************** function pointers **********************/
typedef vector<node>(*exPnd)(node, problem);
typedef nodes(*qFunc)(nodes*, exPnd);

/****************  the algorithm functions  ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes* n, exPnd); // FOR TESTING PURPOSES
vector<node> expand(node, problem);

vector<Slide*> repeated;
bool haveSeen(node);
bool compare(Slide* lhs, Slide* rhs) {
    return *lhs < *rhs;
}

int main() {
    int n = 3; // default grid size n*n
    /* testing these configurations */
    int t[] = {1, 2, 3, 4, 5, 6, 7, 8, 0};          // trivial
    int e[] = {1, 2, 0, 4, 5, 3, 7, 8, 6};          // easy
    int d[] = {0, 1, 2, 4, 5, 3, 7, 8, 6};          // doable
    int ob[] = {8, 7, 1, 6, 0, 2, 5, 4, 3};         // ob= oh boy
    int b[] = {1, 2, 3, 4, 5, 6, 8, 7, 0};          // broken. should not work
    
    Slide *ohBoy = new mhatSlide(ob, n);
    Slide *trivial = new mhatSlide(t,n);
    Slide *easy = new mhatSlide(e, n);
    Slide *doable = new mhatSlide(d, n);

    priority_queue<Slide*> testing;
    testing.push(ohBoy);
    testing.push(trivial);
    testing.push(easy);
    testing.push(trivial);
    testing.push(doable);
    
    while(!testing.empty()) {
        cout << "hn: " << testing.top()->getFn() << endl;
        testing.top()->print();
        testing.pop();
    }
    
    //cout << genSearch(trivial, queueFunc) << endl;
    cout << "repeated size: " << repeated.size()<< endl;
    for (int i = 0; i != repeated.size(); ++i) {
        repeated.at(i)->print();
    }

    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

bool genSearch(problem p, qFunc que) {
    // initialize the queue with the initial state
    nodes *n = new priority_queue<node>;
    n->push(p);
    repeated.push_back(p);
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        cout << "now testing\n";
        n->top()->print();
        cout << "Has highest fn at: " << n->top()->getFn() << endl;
        // are you the goal state
        if (n->top()->isGoal()) return true; // found the solution
        *n = que(n, expand);
    } while (true);
}

// takes a node and expands it using operators
// returns a vector with the expanded nodes
// i.e, a vector containing only valid states
vector<node> expand(node current, problem p) {
    vector<node> newNodes;

    if (current->moveLeft()) {
        newNodes.push_back(new misSlide(*current));
        current->moveRight();           // reset the tile
    }
    if (current->moveRight()) {
        newNodes.push_back(new misSlide(*current));
        current->moveLeft();           // reset the tile
    }
    if (current->moveUp()) {
        newNodes.push_back(new misSlide(*current));
        current->moveDown();           // reset the tile
    }
    if (current->moveDown()) {
        newNodes.push_back(new misSlide(*current));
        current->moveUp();           // reset the tile
    }
    return newNodes;
}

nodes queueFunc(nodes* n, exPnd exp) {
    vector<node> newNodes = exp(n->top(), n->top()); // expand the nodes
    delete n->top();                                  // free memory
    n->pop();                                       //remove expanded element
    // push all the new nodes that were expanded in the expand function
    for (int i = 0; i != newNodes.size(); ++i) {
        // check if we've seen the node before
        if (!haveSeen(newNodes.at(i))){
            // we haven't, so push it onto the queue
            n->push(newNodes.at(i));
            // push previously unseen node onto list of repeated nodes
            repeated.push_back(newNodes.at(i));
        }
    }
    // return them
    return *n;
}


bool haveSeen(node current) {
    for (int i = 0; i != repeated.size(); ++i) {
        if (*repeated.at(i) == *current) {
            cout << "************************\n";
            repeated.at(i)->print();
            cout << "*********\n";
            current->print();
            cout << "************************\n";
            return true;
        }
    }
    return false;
}
