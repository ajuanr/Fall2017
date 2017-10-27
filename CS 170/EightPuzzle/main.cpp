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
typedef priority_queue<node, vector<node>, bool(*)(node, node)> nodes;
typedef vector<node> vecNode;

/*************** function pointers **********************/
typedef vecNode(*exPnd)(node*, problem);
typedef nodes(*qFunc)(nodes*, exPnd);
typedef bool(*ops)();

/****************  the algorithm functions  ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes* n, exPnd); // FOR TESTING PURPOSES
vecNode EXPAND(node*, problem);

vector<Slide> repeated; // the nodes we've already seen

bool haveSeen(node*);
bool cmpUniform(Slide a, Slide b);
bool cmpMhat(Slide a, Slide b);
bool cmpTiles(Slide a, Slide b);


int main() {
    int n = 3; // default grid size n*n
    /* testing these configurations */
    int t[] = {1, 2, 3, 4, 5, 6, 7, 0, 8};          // trivial
    int e[] = {1, 2, 0, 4, 5, 3, 7, 8, 6};          // easy
    int d[] = {0, 1, 2, 4, 5, 3, 7, 8, 6};          // doable
    int ob[] = {8, 7, 1, 6, 0, 2, 5, 4, 3};         // ob= oh boy
    int b[] = {1, 2, 3, 4, 5, 6, 8, 7, 0};          // broken. should not work
    
    int test[] = {1,2,3,4,8,0,7,6,5};
    Slide testing(test,n);
    
    Slide ohBoy(ob, n);
    Slide trivial(t,n);
    Slide easy(e, n);
    Slide doable(d, n);
    Slide broken(b, n);
    
    cout << genSearch(ohBoy, queueFunc) << endl;
    
    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

bool genSearch(problem p, qFunc que) {
    int numExpanded = 0;    // the number of nodes expanded
    int maxNodes=0;           // the maximum number of nodes at one time
    // initialize the queue with the initial state
    nodes *n = new nodes(cmpMhat);
    n->push(p);
    repeated.push_back(p);
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        ++maxNodes;
        // are you the goal state
        if (n->top().isGoal()){
            cout << "The puzzle is done\n";
            n->top().print();
            cout << "Maximum number of nodes in queue "
                 << "at any one time: " << maxNodes << endl;
            return true; // found the solution
        }
        // update max nodes in queue
        maxNodes=(maxNodes > n->size()) ? maxNodes:static_cast<int>(n->size());
        cout << "now testing\n";
        n->top().print();
        cout << "H(n)= : " << n->top().misTiles() << endl;
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

// returns true if a node has been seen previously
bool haveSeen(node *current) {
    for (int i = 0; i != repeated.size(); ++i)
        if (repeated.at(i) == *current) return true;
    return false;
}

/***********************************************************
 *******************Comparison Functions *******************
 * ***************** Return true if a > b ******************
 * Want to priority queue to have smallest elelment on top *
 ********************************************************* */
bool cmpUniform(Slide a, Slide b) {
    return a.getGn() > b.getGn();
}

bool cmpMhat(Slide a, Slide b) {
    return (a.getGn()+a.mhatDist()) > (b.getGn()+b.mhatDist());
}

bool cmpTiles(Slide a, Slide b) {
    return (a.getGn()+a.misTiles()) > (b.getGn()+b.misTiles());
}
