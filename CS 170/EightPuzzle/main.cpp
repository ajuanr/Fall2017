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
typedef Slide node;
typedef Slide problem;
typedef priority_queue<problem> nodes;
typedef vector<node> vecNode;

/*************** function pointers **********************/
typedef vecNode(*exPnd)(node*, problem);
typedef nodes(*qFunc)(nodes*, exPnd);
typedef bool(*ops)();

/****************  the algorithm functions  ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes* n, exPnd); // FOR TESTING PURPOSES
vecNode EXPAND(node*, problem);

vector<Slide> repeated;

bool haveSeen(node*);
bool my_compare(Slide lhs, Slide rhs) {
    return lhs < rhs;
}

int main() {
    int n = 3; // default grid size n*n
    /* testing these configurations */
    int t[] = {1, 2, 3, 4, 5, 6, 7, 0, 8};          // trivial
    int e[] = {1, 2, 0, 4, 5, 3, 7, 8, 6};          // easy
    int d[] = {0, 1, 2, 4, 5, 3, 7, 8, 6};          // doable
    int ob[] = {8, 7, 1, 6, 0, 2, 5, 4, 3};         // ob= oh boy
    int b[] = {1, 2, 3, 4, 5, 6, 8, 7, 0};          // broken. should not work
    
    //int test[] = {0, 1, 3, 4,2,5,7,8,6};
    //Slide testing(test,n);
    
    Slide ohBoy(ob, n);
    Slide trivial(t,n);
    Slide easy(e, n);
    Slide doable(d, n);
    Slide broken(b, n);
    
    priority_queue<Slide> testing;
    testing.push(ohBoy);
    testing.push(trivial);
    testing.push(easy);
    testing.push(trivial);
    testing.push(doable);

//    while(!testing.empty()) {
//        cout << "hn: " << testing.top().getFn() << endl;
//        testing.top().print();
//        testing.pop();
//    }
    
    cout << genSearch(ohBoy, queueFunc) << endl;
    
    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

bool genSearch(problem p, qFunc que) {
    int numExpanded = 0;
    // initialize the queue with the initial state
    nodes *n = new priority_queue<node>;
    n->push(p);
    repeated.push_back(p);
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        cout << "now testing\n";
        n->top().print();
        cout << "G(n)= : " << n->top().getGn() << endl;
        // are you the goal state
        if (n->top().isGoal()) return true; // found the solution
        cout << "number of nodes expanded: " << ++numExpanded << endl;
        *n = que(n, EXPAND);
    } while (true);
}

// takes a node and expands it using operators
// returns a vector with the expanded nodes
// i.e, a vector containing only valid states
vecNode EXPAND(node *current, problem p) {
    vecNode newNodes;
    int i = 0;
    if (current->moveLeft()) {
        newNodes.push_back(*current);
        newNodes.at(i++).incrementG();
        current->moveRight();           // reset the tile
    }
    if (current->moveRight()) {
        newNodes.push_back(*current);
        newNodes.at(i++).incrementG();
        current->moveLeft();           // reset the tile
    }
    if (current->moveUp()) {
        newNodes.push_back(*current);
        newNodes.at(i++).incrementG();
        current->moveDown();           // reset the tile
    }
    if (current->moveDown()) {
        newNodes.push_back(*current);
        newNodes.at(i++).incrementG();
        current->moveUp();           // reset the tile
    }
    return newNodes;
}

nodes queueFunc(nodes* n, exPnd exp) {
    node temp = n->top();
    vecNode newNodes = exp(&temp, n->top()); // expand the nodes

    n->pop();                                       //remove expanded element
    // push all the new nodes that were expanded in the expand function
    for (int i = 0; i != newNodes.size(); ++i) {
        // check if we've seen the node before
        if (!haveSeen(&newNodes.at(i))){
            // we haven't, so push it onto the queue
            n->push(newNodes.at(i));
            // push previously unseen node onto list of repeated nodes
            repeated.push_back(newNodes.at(i));
        }
    }
    // return them
    return *n;
}

// return true if a node has been seen previously
bool haveSeen(node *current) {
    for (int i = 0; i != repeated.size(); ++i)
        if (repeated.at(i) == *current) return true;
    return false;
}
