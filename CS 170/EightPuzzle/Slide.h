#ifndef SLIDE_H
#define SLIDE_H

class Slide{
public:
    Slide();           // create a default grid of size 3x3
    Slide(int);        // create a default grid of size nxn
    Slide(int*, int); // initialize the grid from an array
    
    void print() const;
    
    // operators allowed
    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    
    // Overloaded operators
    bool operator==(const Slide&);
    bool operator!=(const Slide&);
    bool operator[](int index) { return grid[index];}
    bool operator[](int index) const { return grid[index];}
    
private:
    int* grid;      // will hold the n*n grid as a 1-d array
    int inputSize;  // input of size n
    int gridSize;   // grid size of nxn
    int blankPos;   // keep track of where the blank is for moving it around
};

#endif
