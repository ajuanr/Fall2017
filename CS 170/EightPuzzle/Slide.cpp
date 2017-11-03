#include <iostream>
#include <iomanip>          // for setw
#include <cmath>            // for abs
#include <algorithm>        // for swap

#include "Slide.h"

using namespace std;

Slide::Slide(int* input, int size):inputSize(size) {
    gridSize = inputSize * inputSize;
    grid = vecInt(gridSize);
    for (int i = 0; i != gridSize; ++i) {
        grid.at(i) = input[i];
        if (input[i] == 0 ){
            blankPos = i;
        }
    }
}

// return true if lhs == rhs
// assumes we'll only compare two grids that are the same size
bool Slide::operator==(const Slide& rhs) const {
    if (this->grid != rhs.grid)
        return false;
    return true;
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

void Slide::print() const{
    for (int i = 0; i != gridSize; ++i) {
        if ((i != 0) && (i % inputSize == 0))
            cout << endl;
        if (grid.at(i) == 0) cout << "    ";
        else cout << setw(4) << grid.at(i);
    }
    cout << endl << endl;
}

// returns the number of misplaced tiles, not including the blank tile
int Slide::misTiles() const {
    int nMisplaced = 0;             // number of misplaced tiles
    for (int i = 0; i != gridSize; ++i) {
        // check if a tile is misplaced, but ignore the blank tile
        if (grid.at(i) != i+1 && grid.at(i) != 0)
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
        if (grid.at(i) != i+1 && grid.at(i) != 0)
            // pass in the misplaced tile
            dist += mhat(grid.at(i));
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
    displacement = abs(displacement - (n-1)); //n-1 is the position in the array
    // columns shifted over and rows shifted over
    int mDistance = (displacement / inputSize) + (displacement % inputSize);
    return mDistance;
}

int Slide::myHash() const {
    int hash = 0;
    int multiplier = 1;
    for (int i = 0; i != grid.size(); ++i) {
        hash += (grid.at(i) * multiplier++);
    }
    hash *= (blankPos+1); // values [156,1836]
    return hash-156; // want the range to be [0,1680]
}

bool Slide::moveLeft() {
    if ( blankPos % inputSize != 0) {
        --blankPos; // move the blank
        swap(grid.at(blankPos), grid.at(blankPos+1));
        return true;                // move was successful
    }
    return false;                   // move could not be executed
}

bool Slide::moveRight() {
    if (blankPos % inputSize != (inputSize-1)) {
        ++blankPos; // move the blank
        swap(grid.at(blankPos), grid.at(blankPos-1));
        return true;                // move was successful
    }
    return false;                   // move could not be executed
}

bool Slide::moveUp() {
    // blank is already on first row, can't move up
    if (blankPos < inputSize)
        return false;
    
    blankPos -= inputSize;                                  // move the blank
    swap(grid.at(blankPos), grid.at(blankPos+inputSize));   // swap the values
    return true;
}

bool Slide::moveDown() {
    // blank is on last row, can't move down
    if (blankPos >= (inputSize * (inputSize-1)))
        return false;
    
    blankPos += inputSize;                                  // move the blank
    swap(grid.at(blankPos), grid.at(blankPos-inputSize));   // swap the values
    return true;
}
