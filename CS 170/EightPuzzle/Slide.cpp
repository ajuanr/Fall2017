#include <iostream>
#include <iomanip>

#include "Slide.h"

using namespace std;

Slide::Slide():inputSize(3) {
    create(inputSize);
}

Slide::Slide(int n):inputSize(n){
    create(inputSize);
}

Slide::Slide(int* input, int size):inputSize(size) {
    gridSize = inputSize * inputSize;
    grid = new int[gridSize];
    for (int i = 0; i != gridSize; ++i) {
        grid[i] = input[i];
        if (input[i] == 0 ){
            blankPos = i;
        }
    }
}

// initialize the grid
void Slide::create(int n) {
    gridSize = inputSize * inputSize;
    blankPos = gridSize - 1;
    grid = new int[gridSize];
    for (int i = 0; i!= gridSize; ++i) {
        grid[i] = i+1;
    }
    grid[gridSize-1] = 0;
}

void Slide::print() const{
    for (int i = 0; i != gridSize; ++i) {
        if ((i != 0) && (i % inputSize == 0))
            cout << endl;
        if (grid[i] == 0) cout << "    ";
        else cout << setw(4) << grid[i];
    }
    cout << endl << endl;
}

bool Slide::moveLeft() {
    if ( blankPos % inputSize != 0) {
        int temp = grid[--blankPos];
        grid[blankPos] = 0;
        grid[blankPos+1] = temp;
        return true;                // move was successful
    }
    return false;                   // move could not be executed
}

bool Slide::moveRight() {
    if (blankPos % inputSize != (inputSize-1)) {
        int temp = grid[++blankPos];
        grid[blankPos] = 0;
        grid[blankPos-1] = temp;
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
    grid[blankPos] = 0;
    grid[blankPos+inputSize] = temp;
    
    return true;
}

bool Slide::moveDown() {
    // blank is on last row, can't move down
    if (blankPos >= (inputSize * (inputSize-1)))
        return false;
    
    blankPos += inputSize;                  // move the blank
    
    // swap the values
    int temp = grid[blankPos];
    grid[blankPos] = 0;
    grid[blankPos-inputSize] = temp;
    
    return true;
}

// return true if lhs == rhs
// assumes we'll only compare two grids that are the same size
bool Slide::operator==(const Slide& rhs) const {
    // compare every value in both grids
    for (int i = 0; i != this->gridSize; ++i) {
        if (this->grid[i] != rhs[i]) {
            return false;
        }
    }
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
            dist += mhat(grid[i]);
    }
    
    return dist;
}

// calculate the manhattan distance for a single tile
int Slide::mhat(int n) const {
    int dist = 0;
    // find the index where the misplaced value n currently is
    for (int i = 0; i != gridSize; ++i) {
        if (n == grid[i])
            dist = i;
    }
    return dist-(n-1);
}
