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

typedef Slide node;
typedef queue<Slide> nodes;


nodes qFunc(nodes, vector<node>);

nodes expand(node*);

//bool generalSearch(problem p);

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

    nodes testing = expand(ohBoy);
    while (!testing.empty()) {
        testing.front().print();
        testing.pop();
    }
    
    return 0;
}

/**********************************************************
 *****************  function definitions  *****************
 *********************************************************/

nodes expand(node* current) {
    nodes newNodes;
    if (current->moveLeft()) {
        newNodes.push(*current);
        current->moveRight();           // reset the tile
    }
    if (current->moveRight()) {
        newNodes.push(*current);
        current->moveLeft();           // reset the tile
    }
    if (current->moveUp()) {
        newNodes.push(*current);
        current->moveDown();           // reset the tile
    }
    if (current->moveDown()) {
        newNodes.push(*current);
        current->moveUp();           // reset the tile
    }
    return newNodes;
}
