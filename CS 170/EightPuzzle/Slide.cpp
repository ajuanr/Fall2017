#include <iostream>
#include <iomanip>

#include "Slide.h"

using namespace std;

Slide::Slide():inputSize(3) {
    gridSize = inputSize * inputSize;
    blankPos = gridSize - 1;
    grid = new int[gridSize];
    for (int i = 0; i!= gridSize; ++i) {
        grid[i] = i+1;
    }
    grid[gridSize-1] = 0;
}

Slide::Slide(int n):inputSize(n){
    gridSize = inputSize * inputSize;
    blankPos = gridSize - 1;
    grid = new int [gridSize];
    for (int i = 0; i!= gridSize; ++i) {
        grid[i] = i;
    }
}
                         
void Slide::print() const{
    for (int i = 0; i != gridSize; ++i) {
        if ((i != 0) && (i % inputSize == 0))
            cout << endl;
        cout << setw(3) << grid[i];
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
