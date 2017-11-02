// Juan Ruiz
// main.cpp
// CS 170 - Fall 2017
// Eight-puzzle programming problem

// System header files
#include <iostream>     // for I/O
#include <queue>        // for priority queue
#include <map>          // map to hold repeated states
#include <iomanip>

// User header files
#include "Slide.h"

using namespace std;

/******************** typedefs **************************/
typedef Slide node;
typedef Slide problem;
typedef priority_queue<node, vector<node>, bool(*)(node, node)> nodes;
typedef vector<node> vecNode;
typedef map<int, vector<node> > repeatMap;

/*************** function pointers **********************/
typedef vecNode(*exPnd)(node*, problem);
typedef nodes(*qFunc)(nodes*, exPnd);

/**************** the algorithm functions ***************/
bool genSearch(problem, qFunc);
nodes queueFunc(nodes* n, exPnd); // FOR TESTING PURPOSES
vecNode EXPAND(node*, problem);

/***************************functions **********************/
bool haveSeen(node*);
nodes* callHeuristic(int);
void callInfo(int, const node);
int selectSearch();
void output(nodes*, int, int);
problem input();

/*********************** comparison functions ************/
bool cmpUniform(node a, node b);
bool cmpMhat(node a, node b);
bool cmpTiles(node a, node b);

repeatMap repeats;   // holds the states we've already seen

/************************ main *************************/
int main() {
    genSearch(input(), queueFunc);
    return 0;
}

/* *****************  function definitions  *****************/

bool genSearch(problem p, qFunc que) {
    int numExpanded = 0;    // the number of nodes expanded
    int maxNodes=0;           // the maximum number of nodes at one time
    int choice = selectSearch();
    
    //initializing
    nodes *n = callHeuristic(choice); // the queue
    n->push(p);                 // push the problem
    repeats[p.myHash()].push_back(p);
    
    // look for a solution
    do {
        // check if any nodes left in queue
        if (n->empty()) {
            cout << "No solution\n";
            return false; // didn't find solution
        }
        // update maxNodes in queue
        maxNodes=maxNodes > n->size() ? maxNodes : static_cast<int>(n->size());
        callInfo(choice, n->top());
        n->top().print();
        // are you the goal state
        if (n->top().isGoal()){
            output(n, numExpanded, maxNodes);
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
        newNodes.at(i++).incrementGn();
        current->moveRight();           // reset the tile
    }
    if (current->moveRight()) {
        newNodes.push_back(*current);
        newNodes.at(i++).incrementGn();
        current->moveLeft();           // reset the tile
    }
    if (current->moveUp()) {
        newNodes.push_back(*current);
        newNodes.at(i++).incrementGn();
        current->moveDown();           // reset the tile
    }
    if (current->moveDown()) {
        newNodes.push_back(*current);
        newNodes.at(i++).incrementGn();
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
    // look for the hashehd key
    repeatMap::iterator iter = repeats.find(current->myHash());
    if (iter != repeats.end()) {
        // key found, check if puzzle state is there
        for(vector<node>::iterator i = iter->second.begin();
            i != iter->second.end(); ++i) {
            if (*i == *current)
                return true;
        }
    }
    // new hash. Add it to repeats
    repeats[current->myHash()].push_back(*current);
    return false;
}

 /******************Comparison Functions *******************
 * ***************** Return true if a > b ******************
 ** Want priority queue to have smallest elelment on top ***
 ********************************************************* */
bool cmpUniform(node a, node b) {
    return a.getGn() > b.getGn();
}

bool cmpMhat(node a, node b) {
    return (a.getGn() + a.mhatDist()) > (b.getGn() + b.mhatDist());
}

bool cmpTiles(node a, node b) {
    return (a.getGn() + a.misTiles()) > (b.getGn() + b.misTiles());
}
/************************************************************/

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

// print information for the current node being expanded
void callInfo(int choice, const node current) {
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
    cout << " is..." << endl;
}

int selectSearch() {
    int choice=0;
    cout << "Enter your choice of algorithm\n"
            "1. Uniform cost search\n"
            "2. A* with Misplaced tile heurisic\n"
            "3. A* with Manhattan Distance heuristic\n>> ";
    cin >> choice;
    cout << endl;
    return choice;
}

problem input() {
    int n = 3; // default grid size n*n
    /* testing these configurations */
    cout << "Welcome to Juan Ruiz's 8-puzzle solver\n"
       << "Type 1 to use a default puzzle, or 2 to enter your own puzzle:\n>> ";
    int choice;
    cin >> choice;
    if (choice != 2) { // any value not equal to 2 gets the default puzzle
        int t[] = {1, 2, 3, 4, 0, 6, 7, 5, 8};         // traced by hand
        return problem(t, n);
    
    }
    int *values = new int[n*n];             // will hold the data
    int val;
    cout << "Enter your puzzle. Use a zero to represent the blank" << endl;
    const char* ROWS[] = {"first ", "second", "third "};
    for (int i = 0; i != n; ++i) {
        cout  <<  "Enter the " << ROWS[i] << " row. "
        << "Use space or tabs between numbers:\t" << setw(10);
        for (int j = 0; j != n; ++j) {
            cin >> val;
            *(values+(i*n+j)) = val;
        }
    }
    cout << endl;
    problem ret(values, n);
    delete[] values;
    return ret;
}

void output(nodes* q, int numExpanded, int maxNodes) {
    cout << "Goal!!!\n";
    cout << "number of nodes expanded: " << numExpanded << endl;
    cout << "At depth: " << q->top().getGn() << endl;
    cout << "Maximum number of nodes in queue "
    << "at any one time: " << maxNodes << endl;
}
