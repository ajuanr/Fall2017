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
#include <string>                   // to get a line of text
#include <sstream>                  // to parse a line of text
#include <vector>                   // to hold the data.

using namespace std;

typedef vector<float> fVec;         // holds the features
typedef vector<fVec> fvVec;         // holds all the rows
typedef vector<int> iVec;           // holds which features to look at

struct bstFeats{
    bstFeats():accuracy(50){ }
    bstFeats(float a, iVec f):accuracy(a), features(f) { }
    float accuracy;
    iVec features;
};

fvVec readData();
fVec parseLine(const string);
int numFeats(const string);
void print(const fvVec&);
float knn(const fvVec&, const fVec&,const iVec&);
float distance(const fVec&, const fVec&, const iVec&);
fvVec valData(const fvVec&, int, int);
fvVec testData(const fvVec&, int, int);
void classify(const fvVec&, fvVec&, const iVec&);
float featrMean(const fvVec&, int);
float stdDev(const fvVec&, int);
void zNormalize(fvVec&);
float accuracy(const fvVec&, const fvVec&, int);
float vote(const fvVec&, int);
bool cmpFeatures(const bstFeats &a,const bstFeats &b){return a.accuracy>b.accuracy;}
//search stuff
float leaveOneOutCrossValidation(const fvVec&, const iVec&);
void featureSearch(const fvVec&);


/*****************************************************************************/
int main(int argc, const char * argv[]) {
    fvVec data = readData();
    if (!data.empty()) {
        cout << "Normalizing data...";
        zNormalize(data);
        cout << "Done\n\n";

        featureSearch(data);
    }
    else
        cout << "There's no data\n";
    return 0;
}
/*****************************************************************************/

// Read the data and return a vector containing the data
fvVec readData(){
    const string fileName = "CS170Smalltestdata__44.txt";
//    const string fileName = "CS170BIGtestdata__4.txt";
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
float knn(const fvVec &data, const fVec &testing, const iVec &features) {
    fvVec nearestClass;
    int closestClass=-1;
    float smallestDistance = 100;
    for (int i = 0; i != data.size(); ++i) {
        if (testing == data.at(i)) continue; // don't compare testing to itself
        float temp = distance(data.at(i), testing, features);
        if (temp < smallestDistance) {
            smallestDistance = temp;
            closestClass = data.at(i).at(0);
        }
    }
    return closestClass;
}

// calculate the distance between to entries
// assumes x and y are same size;
float distance(const fVec& x, const fVec &y, const iVec &featrs) {
    float distance = 0;
    for(int i = 0; i != featrs.size(); ++i) {
        int c = featrs.at(i);                       // the current feature
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
void classify(const fvVec& val, fvVec &test, const iVec &f) {
        test.at(0).at(0) = knn(val, test.at(0),f);
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
float accuracy(const fvVec& original, const fvVec& tested, int start){
    float output = 0;
    if (original.at(start).at(0) == tested.at(0).at(0)) ++output;
    return output * 100.0;
}

float leaveOneOutCrossValidation(const fvVec& data, const iVec& features) {
    float acc = 0.0;                   // accuracy
    int start = 1, end = start+1;
    float numCorrect=0;
    while (end <= data.size()) {
        fvVec validation = valData(data,start,end);
        fvVec testing = testData(data,start,end);
        
        classify(validation, testing, features);
        acc = accuracy(data, testing, start);
        if (acc == 100) ++numCorrect;
        ++start;
        ++end;
    }
    return numCorrect/data.size()*100.00;
}

void featureSearch(const fvVec& data) {
    iVec features;
    iVec tempFeatures;
    vector<bstFeats> best;
    int bestAtThisLevel;
    for (int i = 1; i != data.at(0).size(); ++i) {
        cout << "on level " << i << endl;
        float bestAccuracy = 50.0;
        for (int j = 1; j != data.at(0).size(); ++j) {
            if (find(features.begin(), features.end(), j) == features.end()) {
                cout << "considering adding: " << j << endl;
                tempFeatures = features;
                tempFeatures.push_back(j);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                if ( accuracy > bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        features.push_back(bestAtThisLevel);
        bstFeats temp(bestAccuracy, features);
        best.push_back(temp);
        sort(best.begin(),best.end(), cmpFeatures);
        cout << "best accuracy: " << bestAccuracy << endl;
        if (best.at(0).accuracy > bestAccuracy) {
            cout << "(WARNING: Accuracy has decresed. "
            "Continuing search in case of local maxima.)\n";
        }
    }
    sort(best.begin(),best.end(), cmpFeatures);
        cout << best.at(0).accuracy << " ";
        copy(best.at(0).features.begin(), best.at(0).features.end(),
             std::ostream_iterator<int>(std::cout, " "));
        cout << endl;
}

