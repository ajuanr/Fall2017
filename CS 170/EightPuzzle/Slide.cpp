#include <iostream>
#include <iomanip> // for setw
#include <cmath>        // for abs

#include "Slide.h"

using namespace std;

// default constructor
Slide::Slide():inputSize(3) {
    create(inputSize);
}

Slide::Slide(int n):inputSize(n){
    create(inputSize);
}

Slide::Slide(int* input, int size):inputSize(size) {
    gridSize = inputSize * inputSize;
    //grid = new int[gridSize];
    grid = vecInt(gridSize);
    for (int i = 0; i != gridSize; ++i) {
        grid.at(i) = input[i];
        if (input[i] == 0 ){
            blankPos = i;
        }
    }
}

// initialize the grid
void Slide::create(int n) {
    gridSize = inputSize * inputSize;
    blankPos = gridSize - 1;
    grid = vecInt(gridSize);
    for (int i = 0; i!= gridSize; ++i) {
        grid.at(i) = i+1;
    }
    grid.at(gridSize-1) = 0;
}

Slide& Slide::operator=(const Slide& rhs) {
    // protect against self assignment
    if (this != &rhs) {
        this->grid = rhs.grid;
        this->inputSize = rhs.inputSize;
        this->gridSize = rhs.gridSize;
        this->blankPos = rhs.blankPos;
        this->gn = rhs.gn;
    }
    return *this;
}

//bool Slide::operator< (const Slide& rhs) const {
//    return this->getFn() > rhs.getFn();
//}
//
//bool Slide::operator> (const Slide& rhs) const {
//    return !(*this < rhs);
//}

void Slide::print() const{
    for (int i = 0; i != gridSize; ++i) {
        if ((i != 0) && (i % inputSize == 0))
            cout << endl;
        if (grid.at(i) == 0) cout << "    ";
        else cout << setw(4) << grid.at(i);
    }
    cout << endl << endl;
}

bool Slide::moveLeft() {
    if ( blankPos % inputSize != 0) {
        int temp = grid[--blankPos];
        grid.at(blankPos) = 0;
        grid.at(blankPos+1) = temp;
        return true;                // move was successful
    }
    return false;                   // move could not be executed
}

bool Slide::moveRight() {
    if (blankPos % inputSize != (inputSize-1)) {
        int temp = grid[++blankPos];
        grid.at(blankPos) = 0;
        grid.at(blankPos-1) = temp;
        return true;                // move was successful
    }
    return false;                   // move could not be executed
    
}

bool Slide::moveUp() {
    // blank is already on first row, can't move up
    if (blankPos < inputSize)
        return false;
    
    blankPos -= inputSize;                  // move the blank
    
    // swap the values
    int temp = grid[blankPos];
    grid.at(blankPos) = 0;
    grid.at(blankPos+inputSize) = temp;
    
    return true;
}

bool Slide::moveDown() {
    // blank is on last row, can't move down
    if (blankPos >= (inputSize * (inputSize-1)))
        return false;
    blankPos += inputSize;                  // move the blank
    // swap the values
    int temp = grid[blankPos];
    grid.at(blankPos) = 0;
    grid.at(blankPos-inputSize) = temp;
    return true;
}

// return true if lhs == rhs
// assumes we'll only compare two grids that are the same size
bool Slide::operator==(const Slide& rhs) const {
    // compare every value in both grids
    for (int i = 0; i != this->gridSize; ++i)
        if (this->grid != rhs.grid)
            return false;
    return true;
}

// returns the number of misplaced tiles, not including the blank tile
int Slide::misTiles() const {
    int nMisplaced = 0;             // number of misplaced tiles
    for (int i = 0; i != gridSize; ++i) {
        // check if a tile is misplaced, but ignore the blank tile
        if (grid[i] != i+1 && grid[i] != 0)
            ++nMisplaced;
    }
    return nMisplaced;
}

// returns the sum of the manhattan distances for the grid
int Slide::mhatDist() const {
    int dist = 0;
    for (int i = 0; i != gridSize; ++i) {
        // only need to calculate the distance if tile is in wrong location
        // ignore the blank tiles
        if (grid[i] != i+1 && grid[i] != 0)
            // pass in the misplaced tile
            dist += mhat(grid[i]);
    }
    return dist;
}

// calculate the manhattan distance for a single tile
int Slide::mhat(int n) const {
    int displacement = 0;
    // find the index where the misplaced value n currently is
    for (int i = 0; i != gridSize; ++i)
        if (n == grid.at(i))
            displacement = i;
    displacement = abs(displacement - (n-1)); // n-1 is the position in the array
    int mDistance = (displacement / inputSize) + (displacement % inputSize);
    return mDistance;
}
