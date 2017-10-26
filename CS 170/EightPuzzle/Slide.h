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
    void incrementG() {++gn;}
    int getGn() const {return gn;} // returns the g(n) value
    virtual int getFn() const {return mhatDist();}
    
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
    
private:
    vecInt grid;      // will hold the n*n grid as a 1-d array
    const vecInt goal = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    int inputSize;  // input of size n
    int gridSize;   // grid size of nxn
    int blankPos;   // keep track of where the blank is for moving it around
    int gn=1;         // cost to move tile
    
    // utility functions
    void create(int);           // for constructor
    int mhat(int) const;      // for mhatDist heuristic
};

class mhatSlide: public Slide {
public:
    mhatSlide(int* a,int n):Slide(a,n){}
    mhatSlide(const Slide& rhs): Slide(rhs){}
    int getFn()const  {return mhatDist();}
};

class misSlide: public Slide {
public:
    misSlide(int* a, int n):Slide(a,n){}
    misSlide(const Slide& rhs): Slide(rhs){}
    int getFn() const {return misTiles();}
};

#endif
