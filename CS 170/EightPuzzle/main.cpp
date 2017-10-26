// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>
#include <queue>        // FIFO
#include <vector>       // stores the expanded nodes before added to queue
#include <map>

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

/****************  the algorithm functions  ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes* n, exPnd); // FOR TESTING PURPOSES
vector<node> expand(node, problem);

vector<node> repeated;
bool haveSeen(node);

int main() {
    int n = 3; // default grid size n*n
    /* testing these configurations */
    int t[] = {1, 2, 3, 4, 5, 6, 7, 8, 0};          // trivial
    int e[] = {1, 2, 0, 4, 5, 3, 7, 8, 6};          // easy
    int d[] = {0, 1, 2, 4, 5, 3, 7, 8, 6};          // doable
    int ob[] = {8, 7, 1, 6, 0, 2, 5, 4, 3};         // ob= oh boy
    int b[] = {1, 2, 3, 4, 5, 6, 8, 7, 0};          // broken. should not work
    
    Slide *ohBoy = new Slide(ob, n);
    Slide *trivial = new Slide(t,n);
    Slide *easy = new Slide(e, n);
    Slide *doable = new Slide(d, n);
    Slide *broken = new Slide(b, n);

    
    
//    vector<node> result = expand(ohBoy, ohBoy);
//    for (int i = 0; i != result.size(); ++i) {
//        result.at(i)->print();
//        delete result.at(i); // free memory
//    }

    //cout << genSearch(broken, queueFunc) << endl;
    Slide *mhatTest = new mhatSlide(e,n);
    Slide *misTest = new misSlide(e,n);
    Slide *mhatTest2 = new mhatSlide(ob,n);
    cout << easy->mhatDist() << endl;
    cout << mhatTest->getFn() << endl;
    cout << misTest->getFn() << endl;
    cout << (*ohBoy < *easy) << endl;
    
    
    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

bool genSearch(problem p, qFunc que) {
    Slide *goal = new Slide(p->getInputSize()); // goal state
    
    // initialize the queue with the initial state
    nodes *n = new queue<node>;
    n->push(p);
    
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        cout << "now testing\n";
        node old = n->front();
        n->front()->print();
        
        // are you the goal state
        if (*old == *goal) return true; // found the solution
        *n = que(n, expand);
    } while (true);
}

// takes a node and expands it using operators
// returns a vector with the expanded nodes
// i.e, a vector containing only valid states
vector<node> expand(node current, problem p) {
    vector<node> newNodes;

    if (current->moveLeft()) {
        newNodes.push_back(new Slide(*current));
        current->moveRight();           // reset the tile
    }
    if (current->moveRight()) {
        newNodes.push_back(new Slide(*current));
        current->moveLeft();           // reset the tile
    }
    if (current->moveUp()) {
        newNodes.push_back(new Slide(*current));
        current->moveDown();           // reset the tile
    }
    if (current->moveDown()) {
        newNodes.push_back(new Slide(*current));
        current->moveUp();           // reset the tile
    }
    return newNodes;
}

nodes queueFunc(nodes* n, exPnd exp) {
    vector<node> newNodes = exp(n->front(), n->front()); // expand the nodes
    delete n->front();                                  // free memory
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
            return true;
        }
    }
    return false;
}
