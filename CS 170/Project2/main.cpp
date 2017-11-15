//
//  main.cpp
//  CS 170 Project 2
//
//  Created by Juan Ruiz on 11/11/17.
//  Copyright © 2017 Juan Ruiz. All rights reserved.
//

#include <algorithm>                // for sort function
#include <cmath>                    // for square root
#include <fstream>                  // for opening files
#include <iomanip>                  // for set width
#include <iostream>                 // for I/O
#include <map>
#include <string>                   // to get a line of text
#include <sstream>                  // to parse a line of text
#include <vector>                   // to hold the data.

using namespace std;

typedef vector<float> fVec;         // holds the features
typedef vector<fVec> fvVec;         // holds all the rows
typedef vector<int> iVec;           // holds which features to look at

struct bestFeatures{
    float accuracy;
    fVec features;
};

fvVec readData();
fVec parseLine(const string);
int numFeats(const string);
void print(const fvVec&);
float knn(const fvVec&, const fVec&, int);
float distance(const fVec&, const fVec&);
float distance(const fVec&, const fVec&, const iVec&);
fvVec valData(const fvVec&, int, int);
fvVec testData(const fvVec&, int, int);
void classify(const fvVec&, fvVec&, int);
float featrMean(const fvVec&, int);
float stdDev(const fvVec&, int);
void zNormalize(fvVec&);
float accuracy(const fvVec&, const fvVec&, int, int);
float vote(const fvVec&, int);
float bestKnnK(const fvVec&, int);
bool myCompare(fVec a, fVec b) { return a.at(0) < b.at(0);}

/*****************************************************************************/
int main(int argc, const char * argv[]) {
    fvVec data = readData();
    if (!data.empty()) {
        cout << "Normalizing data...";
        zNormalize(data);
        cout << "Done\n";

        int start = 0, end = 25;
        fvVec validation = valData(data,start,end);
        fvVec testing = testData(data,start,end);
        classify(validation, testing, bestKnnK(data, end - start));
        cout << accuracy(data, testing, start, end) << endl;
    }
    else
        cout << "There's no data\n";
    return 0;
}
/*****************************************************************************/

// Read the data and return a vector containing the data
fvVec readData(){
//    const string fileName = "CS170Smalltestdata__44.txt";
    const string fileName = "CS170BIGtestdata__4.txt";
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
float knn(const fvVec &data, const fVec &testing, int k) {
    fvVec nearestClass;
    iVec features= {1,2,3,4,5,6,7,8,9,10,11};   // TESTING
    for (int i = 0; i != data.size(); ++i) {
        if (testing == data.at(i)) continue; // don't compare testing to itself
        float temp = distance(data.at(i), testing, features);
        fVec tempVec = {temp, data.at(i).at(0)};
        nearestClass.push_back(tempVec);
        }
    sort(nearestClass.begin(), nearestClass.end(), myCompare);
    return vote(nearestClass,k);
}

// input: vector with nearest classes, how many nearest neighbors to check
// returns the class with the most votes
float vote(const fvVec &poll, int votes) {
    int results[3] = {0}; // [0] is unused; [1] = class 1; [2] = class 2
    for (int i = 0; i != votes; ++i) {
        ++results[static_cast<int>(poll.at(i).at(1))];
    }
    // return whichever class had more votes
    return results[1] > results[2] ? 1 : 2;
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

// calculate the distance between to entries
// assumes x and y are same size;
float distance(const fVec& x, const fVec &y, const iVec &featrs) {
    float distance = 0;
    // start at one to ignore the class identifier
    for(int i = 0; i != featrs.size(); ++i) {
        int c = featrs.at(i);       // the current feature
        distance += ((y.at(c) - x.at(c)) * (y.at(c) - x.at(c)));
    }
    return sqrt(distance);
}

// returns the data that will be used for testing
fvVec valData(const fvVec &data, int start, int end) {
    fvVec output;
    for (int i = 0; i != data.size(); ++i) {
        if (i>=start && i < end) continue;      // don't copy [start, end)
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
// alters the class values in the test data
void classify(const fvVec& val, fvVec &test, int k) {
    for (int i = 0; i != test.size(); ++i) {
        test.at(i).at(0) = knn(val, test.at(i), k);
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

// returns the accurary of the tested data
float accuracy(const fvVec& original, const fvVec& tested, int start, int end){
    float output = 0;
    for (int i = start, j=0; i != end; ++i,++j) {
        if (original.at(i).at(0) == tested.at(j).at(0)) ++output;
    }
    return output / (end-start) * 100.0;
}

// returns the best k-val for use in knn
float bestKnnK(const fvVec &data, int kSize) {
    typedef map<int, int> ivMap;
    ivMap results;
    int range = kSize;
    int start = 0;
    int end = start+range;
    int bestKval = -1;  // will have best accuracy for any range
    // loop through the data set in range size increments
    while ((end <= data.size()) && (start < data.size())) {
        float maxAcc = 0;
        // test k values from [1, data size)
        for (int i = 1; i < data.at(i).size(); i = i+2){
            fvVec validation = valData(data,start,end);
            fvVec testing = testData(data,start,end);
            classify(validation, testing, i);
            int temp = accuracy(data, testing, start, end);
            // found a k-val with better accuracy
            if (temp > maxAcc ) {
                maxAcc = temp;
                bestKval = i;
            }
        }
        results[bestKval]++;
        // check the next range
        start = end;
        end = start+range;
    }
    
    int numSeen = -1;               // number of times k-val was seen
    bestKval = -1;
    // find which k-val was seen the most
    for (ivMap::iterator i = results.begin(); i != results.end(); ++i) {
        if (i->second > numSeen) {
            bestKval = i->first;
            numSeen = i->second;
        }
    }
    return bestKval;
}
