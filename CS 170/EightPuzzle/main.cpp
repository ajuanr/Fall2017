// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>
#include <queue>        // FIFO
#include <vector>       // stores the expanded nodes before added to queue
#include <map>
#include <ctime>

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
typedef vector<bool(*)()> operators;typedef map<int, vector<Slide> > repeatMap;

/*************** function pointers **********************/
typedef vecNode(*exPnd)(node*, problem);
typedef nodes(*qFunc)(nodes*, exPnd);

/****************  the algorithm functions  ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes* n, exPnd); // FOR TESTING PURPOSES
vecNode EXPAND(node*, problem);

/***************************functions **********************/
bool haveSeen(node*);
nodes* callHeuristic(int);
void callInfo(int, node);

/*********************** comparison functions ************/
bool cmpUniform(Slide a, Slide b);
bool cmpMhat(Slide a, Slide b);
bool cmpTiles(Slide a, Slide b);

int repeats[1861] = {0};
repeatMap repeat;

/************************ main *************************/
int main() {
    int n = 3; // default grid size n*n
    /* testing these configurations */
    int t[] = {1, 2, 3, 4, 5, 6, 7, 0, 8};          // trivial
    int e[] = {1, 2, 0, 4, 5, 3, 7, 8, 6};          // easy
    int d[] = {0, 1, 2, 4, 5, 3, 7, 8, 6};          // doable
    int ob[] = {8, 7, 1, 6, 0, 2, 5, 4, 3};         // ob= oh boy
    int b[] = {1, 2, 3, 4, 5, 6, 8, 7, 0};          // broken. should not work
    int w[] = {8, 6, 7, 2, 5, 4, 3, 0, 1};          // worst case
    
    int test[] = {1,2,3,4,8,0,7,6,5};
    Slide testing(test,n);
    
    Slide ohBoy(ob, n);
    Slide trivial(t,n);
    Slide easy(e, n);
    Slide doable(d, n);
    Slide broken(b, n);
    Slide worst(w,n);
    
    int t1 = time(0);
    cout << genSearch(ohBoy, queueFunc) << endl;
    int t2 = time(0);
    cout << "time: " <<  t2-t1 << endl;
    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

bool genSearch(problem p, qFunc que) {
    int numExpanded = 0;    // the number of nodes expanded
    int maxNodes=0;           // the maximum number of nodes at one time
    int choice = 2;
    
    //initializing
    nodes *n = callHeuristic(choice); // the queue
    n->push(p);                 // push the problem
    repeat[p.myHash()].push_back(p);
    
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) return false; // didn't find solution
        // update maxNodes in queue
        maxNodes=maxNodes > n->size() ? maxNodes : static_cast<int>(n->size());
        callInfo(choice, n->top());
        n->top().print();
        // are you the goal state
        if (n->top().isGoal()){
            cout << "The puzzle is done\n";
            n->top().print();
            cout << "number of nodes expanded: " << numExpanded << endl;
            cout << "At depth: " << n->top().getDepth() << endl;
            cout << "Maximum number of nodes in queue "
                 << "at any one time: " << maxNodes << endl;
            return true; // found the solution
        }
        ++numExpanded; // if here, a node had to be expanded
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
        newNodes.at(i).incrementGn();
        newNodes.at(i++).incrementDepth();
        current->moveRight();           // reset the tile
    }
    if (current->moveRight()) {
        newNodes.push_back(*current);
        newNodes.at(i).incrementGn();
        newNodes.at(i++).incrementDepth();
        current->moveLeft();           // reset the tile
    }
    if (current->moveUp()) {
        newNodes.push_back(*current);
        newNodes.at(i).incrementGn();
        newNodes.at(i++).incrementDepth();
        current->moveDown();           // reset the tile
    }
    if (current->moveDown()) {
        newNodes.push_back(*current);
        newNodes.at(i).incrementGn();
        newNodes.at(i++).incrementDepth();
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
        }
    }
    // return them
    return *n;
}

// returns true if a node has been seen previously
bool haveSeen(node *current) {
    repeatMap::iterator iter = repeat.find(current->myHash());
    if (iter != repeat.end()) {
        for(vector<Slide>::iterator i = iter->second.begin();
            i != iter->second.end(); ++i) {
            if (*i == *current)
                return true;
        }
    }
    repeat[current->myHash()].push_back(*current);
    return false;
}

// call the appropriate heuristic
nodes* callHeuristic(int choice) {
    switch(choice) {
        case 1:
            return new nodes(cmpUniform);
            break;
        case 2:
            return new nodes(cmpTiles);
            break;
        case 3:
            return new nodes(cmpMhat);
            break;
        default:
            return new nodes(cmpUniform);
    }
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
    // check for ties in F(n). G(n) all equal so prefer smaller h(n)
    if (a.getGn() + a.mhatDist() == (b.getGn() + b.mhatDist()))
        return (a.mhatDist() > b.mhatDist());
    return (a.getGn() + a.mhatDist()) > (b.getGn() + b.mhatDist());
}

bool cmpTiles(Slide a, Slide b) {
    // check for ties in F(n). G(n) all equal so prefer smaller h(n)
    if (a.getGn() + a.misTiles() == (b.getGn() + b.misTiles()))
        return (a.misTiles() > b.misTiles());
    return (a.getGn() + a.misTiles()) > (b.getGn() + b.misTiles());
}
/************************************************************/

// print information for the current node being expanded
void callInfo(int choice, node current) {
    cout << "The best state to expand with g(n)=" << current.getGn();
    switch(choice) {
        case 1:
            cout << " and h(n)=" << current.uniCost();
            break;
        case 2:
            cout << " and h(n)=" << current.misTiles();
            break;
        case 3:
            cout << " and h(n)=" << current.mhatDist();
            break;
        default:
            cout << " and h(n)=" << current.uniCost();
            break;
    }
    cout << endl;
}
