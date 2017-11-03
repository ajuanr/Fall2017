#ifndef SLIDE_H
#define SLIDE_H

#include <vector>

typedef std::vector<int> vecInt;

class Slide{
public:
    Slide(int*, int);       // Initialize grid from array
    
    // some functions
    void print() const;
    bool isGoal() const {return this->grid == goal;}
    int myHash() const;                 // hash a puzzle
    void incrementGn() {++gn;}          // increment the G(n) value
    int getGn() const {return gn;} // returns the g(n) value
    
    // operators allowed
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    
    // Overloaded operators
    bool operator== (const Slide&) const;
    Slide& operator= (const Slide&);

    // get heuristic values
    int misTiles() const;
    int mhatDist() const;
    int uniCost() const { return 0;} // h(n) set to zero
    
private:
    vecInt grid;                     // holds the n*n grid as a 1d array
    int inputSize;                  // input of size n
    int gridSize;                   // grid size of nxn
    int blankPos;                   // where the blank
    int gn=0;                       // number times blank moved
    const vecInt goal = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    
    int mhat(int) const;   // manhattan distance for single tile
};

#endif
