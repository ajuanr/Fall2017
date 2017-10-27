#ifndef SLIDE_H
#define SLIDE_H

#include <vector>

typedef std::vector<int> vecInt;

class Slide{
public:
    Slide();                // create a default grid of size 3x3
    Slide(int);             // create a default grid of size nxn
    Slide(int*, int);       // initialize the grid from an array
    
    int getInputSize() const { return inputSize;}
    void print() const;
    bool isGoal() const {return this->grid == goal;}
    
    // operators allowed
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    
    // Overloaded operators
    bool operator== (const Slide&) const;
    Slide& operator= (const Slide&);
    virtual bool operator< (const Slide&) const;
    virtual bool operator> (const Slide&) const;

    //heuristics
    int misTiles() const;
    int mhatDist() const;
    int uniCost() const { return 0;} // h(n) set to zero
    
    bool compareMhat(const Slide&, const Slide&) const;
    
    void incrementG() {++gn;}
    int getGn() const {return gn;} // returns the g(n) value
    virtual int getFn() const {return getGn() + misTiles();}
    
private:
    vecInt grid;      // will hold the n*n grid as a 1-d array
    const vecInt goal = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    int inputSize;  // input of size n
    int gridSize;   // grid size of nxn
    int blankPos;   // keep track of where the blank is for moving it around
    int gn=0;         // cost to move tile
    std::vector<bool(*)()> operations;
    
    // utility functions
    void create(int);           // for constructor
    int mhat(int) const;      // for mhatDist heuristic
};

#endif
