#include <iostream>
#include <iomanip>

#include "Slide.h"

using namespace std;

Slide::Slide():inputSize(3) {
    gridSize = inputSize * inputSize;
    blankPos = gridSize - 1;
}

Slide::Slide(int n):inputSize(n){
    gridSize = inputSize * inputSize;
    blankPos = gridSize - 1;
}
                         
void Slide::print() const{
    for (int i = 0; i != gridSize; ++i) {
        if (i!= 0 && (i % inputSize) == 0)
            cout << endl;
        cout << setw(3) << i;
    }
    cout << endl;
}
