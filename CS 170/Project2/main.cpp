//
//  main.cpp
//  CS 170 Project 2
//
//  Created by Juan Ruiz on 11/11/17.
//  Copyright © 2017 Juan Ruiz. All rights reserved.
//

#include <cmath>                // for square root
#include <fstream>              // for opening files
#include <iostream>             // for I/O
#include <string>               // to get a line of text
#include <sstream>              // to parse a line of text
#include <vector>               // to hold the data.

using namespace std;

typedef vector<float> fVec;         // vector of floats
typedef vector<fVec> fvVec;         // vector of vector-floats

fvVec readData();
fVec parseLine(const string);
int numFeats(const string);
void print(const fvVec&);
float kNearest(const fvVec&, const fVec&);
float distance(const fVec&, const fVec&);



int main(int argc, const char * argv[]) {
    fvVec data = readData();
    print(data);
    float closest = kNearest(data, data.at(22));
    cout << closest << endl;
    cout << endl;
    return 0;
}

// Read the data and return a vector containing the data
fvVec readData(){
    const string fileName = "CS170Smalltestdata__44.txt";
    ifstream input;
    input.open(fileName, ifstream::in);
    fvVec output;
    if (!input.is_open()) {
        cout << "File failed to open\n";
        return output;
    }
    string line;
    while ( getline (input,line) ) {
        output.push_back(parseLine(line));
    }
    input.close();
    return output;
}

// Parse a single line. Return a vector containing the feature data
vector<float> parseLine(const string line) {
    stringstream floats;
    floats << line;
    fVec output;
    int index = 0;
    float *temp = new float[numFeats(line)];
    while(floats>>temp[index++]) {
        output.push_back(temp[index-1]);
    }
    delete [] temp;
    
    return output;
}

// Returns a count of the number of features on a line
int numFeats(const string line) {
    stringstream output;
    output << line;
    int count=0;
    float word;
    while(output>>word) {
        ++count;
    }
    return count;
}

// print the data table
void print(const fvVec &data) {
    for (int i = 0; i != data.size(); ++i) {
        for (int j = 0; j != data.at(i).size(); ++j) {
            cout << data.at(i).at(j) << " ";
        }
        cout << endl;
    }
}

float kNearest(const fvVec &data, const fVec &testing) {
    fVec nearest;
    float closest = 10000; // should be greater > any nearest neighbor
    for (int i = 1; i != data.size(); ++i) {
        if (data.at(i) == testing){ continue; }
        // start at 1 since we're ingoring the class identifier
        for (int j = 1; j!=data.at(i).size(); ++j) {
            float temp = distance(data.at(i), testing);
            if (temp < closest && temp != 0){
                closest = temp;
                nearest = data.at(i);
            }
        }
    }
    cout << "distance is: " << closest << endl;
    return nearest.at(0);
}

// calculate the distance between to entries
// assumes x and y are same size;
float distance(const fVec& x, const fVec &y) {
    float distance = 0;
    for (int i = 0; i != x.size(); ++i) {
        distance += ((y.at(i) - x.at(i)) * (y.at(i) - x.at(i)));
    }
    return sqrt(distance);
}
