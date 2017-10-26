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
    
    // operators allowed
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    
    // Overloaded operators
    bool operator== (const Slide&) const;
    int& operator[](int index) { return grid.at(index);}
    const int& operator[](int index) const { return grid.at(index);}
    virtual bool operator< (const Slide&) const;
    virtual int getFn() const { return gn;}
    int getGn() const {return gn;} // returns the g(n) value
    
    //heuristics
    int misTiles() const;
    int mhatDist() const;
    
private:
    vecInt grid;      // will hold the n*n grid as a 1-d array
    int inputSize;  // input of size n
    int gridSize;   // grid size of nxn
    int blankPos;   // keep track of where the blank is for moving it around
    const int gn=1;         // cost to move tile
    
    // utility functions
    void create(int);           // for constructor
    int mhat(int) const;      // for mhatDist heuristic
};

class mhatSlide: public Slide {
public:
    mhatSlide(int* a,int n):Slide(a,n){}
    int getFn()const  {return getGn()+mhatDist();}
    bool operator< (const Slide& rhs) const {return this->getFn()<rhs.getFn();}
};

class misSlide: public Slide {
public:
    misSlide(int* a, int n):Slide(a,n){}
    int getFn() const {return getGn()+misTiles();}
    bool operator< (const Slide& rhs) const {return this->getFn()<rhs.getFn();}
};

#endif
