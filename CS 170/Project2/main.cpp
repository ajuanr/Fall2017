//
//  main.cpp
//  CS 170 Project 2
//
//  Created by Juan Ruiz on 11/11/17.
//  Copyright © 2017 Juan Ruiz. All rights reserved.
//

#include <cmath>                // for square root
#include <fstream>              // for opening files
#include <iomanip>              // for set width
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
fvVec valData(const fvVec&, int, int);
fvVec testData(const fvVec&, int, int);
void classify(const fvVec&, fvVec&);
float featrMean(const fvVec&, int);
float stdDev(const fvVec&, int);
void zNormalize(fvVec&);
float accuracy(const fvVec&, const fvVec&, int, int);

int main(int argc, const char * argv[]) {
    fvVec data = readData();
    //print(data);
    
    cout << "Normalizing data...";
    zNormalize(data);
    cout << "Done\n";
    
    int start=0;
    int end= start+10;
    fvVec validation = valData(data,start,end);
    fvVec testing = testData(data,start,end);

    cout << "***************************************************************\n";
    classify(validation, testing);
    print(testing);
    
    cout << accuracy(data, testing, start, end) << endl;;
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

// prints the data table
void print(const fvVec &data) {
    for (int i = 0; i != data.size(); ++i) {
        for (int j = 0; j != data.at(i).size(); ++j) {
            cout << setprecision(8) << setw(13) << data.at(i).at(j);
        }
        cout << endl;
    }
}

// input: dataset, features to compare
// returns class of nearest value
float kNearest(const fvVec &data, const fVec &testing) {
    float nearest=-1; // invalid value
    float closest = 10000; // should be greater > any nearest neighbor
    for (int i = 0; i != data.size(); ++i) {
            float temp = distance(data.at(i), testing);
            if (temp < closest && temp != 0){
                closest = temp;
                nearest = data.at(i).at(0);
            }
        }
    return nearest;
}

// calculate the distance between to entries
// assumes x and y are same size;
float distance(const fVec& x, const fVec &y) {
    float distance = 0;
    // start at one to ignore the class identifier
    for (int i = 1; i != x.size(); ++i) {
        distance += ((y.at(i) - x.at(i)) * (y.at(i) - x.at(i)));
    }
    return sqrt(distance);
}

fvVec valData(const fvVec &data, int start, int end) {
    fvVec output;
    for (int i = 0; i != data.size(); ++i) {
        // don't copy [start, end)
        if (i>=start && i < end) continue;
        output.push_back(data.at(i));
    }
    return output;
}

// returns data that will be used for testing purposes
fvVec testData(const fvVec &data, int start, int end) {
    fvVec output;
    for (int i = start; i != end; ++i) {
        output.push_back(data.at(i));
    }
    // hide the class
    for (int i = 0; i != output.size(); ++i) {
        output.at(i).at(0) = -1;
    }
    return output;
}

// input: validation data, test data
// alters the values in the test data
void classify(const fvVec& val, fvVec &test) {
    for (int i = 0; i != test.size(); ++i) {
        test.at(i).at(0) = kNearest(val, test.at(i));
    }
}

// input: the dataset and the feature whose mean to calculate
// returns the mean of the feature
float featrMean(const fvVec& data, int feature) {
    float sum = 0.0;
    for (int i = 0; i != data.size(); ++i) {
        sum += data.at(i).at(feature);
    }
    return sum/data.size();
}

// input: the dataset and the feature whose std. deviation to calulate
// returns the std deviation of the feature
float stdDev(const fvVec& data, int featr) {
    float mu = featrMean(data, featr);
    float sum = 0.0;
    for (int i = 0; i != data.size(); ++i) {
    sum += ((data.at(i).at(featr)-mu) * (data.at(i).at(featr)-mu) / data.size());
    }
    return sqrt(sum);
    
}

// input: the dataset
// alters the datset, with the normalized values, except for the class column
void zNormalize(fvVec& data) {
    for (int i = 0; i != data.size(); ++i) {
        for (int j = 1; j != data.at(i).size(); ++j) {
            data.at(i).at(j) -= featrMean(data, j);
            data.at(i).at(j) /= stdDev(data, j);
        }
    }
}

float accuracy(const fvVec& original, const fvVec& tested, int start, int end){
    float output = 0;
    for (int i = start, j=0; i != end; ++i,++j) {
        if (original.at(i).at(0) == tested.at(j).at(0)) ++output;
    }
    return output / (end-start);
}
