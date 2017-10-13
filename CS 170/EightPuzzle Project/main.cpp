//
//  main.cpp
//  EightPuzzle
//
//  Created by Juan Ruiz on 10/13/17.
//  Copyright © 2017 Juan Ruiz. All rights reserved.
//

#include <iostream>


using namespace std;

int** createGrid(int); // creates the nxn grid where tiles are placed
void fillGrid(int**, int); // the tiles from 1 to [(n x ) - 1]
void printGrid(int**, int);

int main(int argc, const char * argv[]) {
    int **grid = createGrid(3);
    fillGrid(grid, 3);
    printGrid(grid, 3);
    return 0;
}


int** createGrid(int size) {
    int** grid = new int* [size]; // create a 2-d array
    for (int i = 0; i != size; ++i) {
        grid[i] = new int [size];      // initialize the rows with size cols.
    
    }
    return grid;
}


void fillGrid(int** grid, int size) {
    for (int i = 0; i != size; ++i) {
        for (int j = 0; j != size; ++j) {
            grid[i][j] = (i+j);
        }
    }

}

void printGrid(int** grid, int size) {
    for (int i = 0; i != size; ++i) {
        for (int j = 0; j != size; ++j) {
            cout <<grid[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}
