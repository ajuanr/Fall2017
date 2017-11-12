//
//  main.cpp
//  CS 170 Project 2
//
//  Created by Juan Ruiz on 11/11/17.
//  Copyright © 2017 Juan Ruiz. All rights reserved.
//

#include <fstream>              // for openeing files
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

typedef vector<float> fVec;         // vector of floats
typedef vector<fVec> fvVec;         // vector of vector-floats

fvVec readData();
fVec parseLine(const string);
int numFeats(const string);
void print(const fvVec);

int main(int argc, const char * argv[]) {
    fvVec data = readData();
    print(data);
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
void print(const fvVec data) {
    for (int i = 0; i != data.size(); ++i) {
        for (int j = 0; j != data.at(i).size(); ++j) {
            cout << data.at(i).at(j) << " ";
        }
        cout << endl;
    }
}
