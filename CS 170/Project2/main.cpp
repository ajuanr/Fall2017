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
#include <map>                      // to hold the distances
#include <string>                   // to get a line of text
#include <sstream>                  // to parse a line of text
#include <vector>                   // to hold the data.

using namespace std;

typedef vector<float> fVec;         // holds the features
typedef vector<fVec> vfVec;         // holds all the rows
typedef vector<int> iVec;           // holds which features to look at
typedef map<float, int> fiMap;

struct bstFeats{
    bstFeats():accuracy(50){ }
    bstFeats(float a, iVec f):accuracy(a), features(f) { }
    float accuracy;
    iVec features;
};

vfVec readData();
fVec parseLine(const string);
void print(const vfVec&);
void print(const iVec&);
float defaultAverage(const vfVec&);
int chooseBestK(const vfVec&, const vfVec&, const fVec&, const iVec&, int);
int knn(const vfVec&, const fVec&,const iVec&, int);
float distance(const fVec&, const fVec&, const iVec&);
fVec validationData(const vfVec&, int);
vfVec trainingData(const vfVec&, int);
void classify(const vfVec&, fVec&, const iVec&, int);
float featrMean(const vfVec&, int);
float stdDev(const vfVec&, int);
void zNormalize(vfVec&);
bool accurate(const vfVec&, const fVec&, int);
int vote(const fiMap&, int);
bool cmpFeatures(const bstFeats &a,const bstFeats &b)
    {return a.accuracy>b.accuracy;}
//search stuff
float leaveOneOutCrossValidation(const vfVec&, const iVec&);
void featureSearch(const vfVec&);

/*****************************************************************************/
int main(int argc, const char * argv[]) {
    vfVec data = readData();
    if (!data.empty()) {
        print(data);
        cout << "Please wait while I normalize the data...   ";
        zNormalize(data);

        cout << "Done\n\n";

        cout << "This dataset has " << data.at(0).size()-1 <<
        " features (not including the class attribute) with " <<
        data.size() << " instances.\n\n";

        cout << "Beginning Search\n\n";
        featureSearch(data);
    }
    else
        cout << "There's no data\n";
    return 0;
}
/*****************************************************************************/

// Read the data and return a vector containing the data
vfVec readData(){
//    const string fileName = "CS170Smalltestdata__44.txt";
//    const string fileName = "CS170BIGtestdata__4.txt";
//    const string fileName = "leaf.txt";
//    const string fileName = "wine.txt";
    const string fileName = "DataUserModeling.txt";
    ifstream input;
    input.open(fileName, ifstream::in);
    vfVec output;
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
    stringstream features;
    features << line;
    fVec output;
    float temp;
    while(features>>temp) {output.push_back(temp);}
    return output;
}

// prints the data table
void print(const vfVec &data) {
    for (int i = 0; i != data.size(); ++i) {
        for (int j = 0; j != data.at(i).size(); ++j) {
            cout << setprecision(8) << setw(13) << data.at(i).at(j);
        }
        cout << endl;
    }
}

// used to print out the features that were checked/used
void print(const iVec &features) {
    if (features.empty()) cout << "{ }";
    cout << "{" << features.at(0);
    for (int i = 1; i != features.size(); ++i) {
        cout << " " << features.at(i);
    }
    cout << "}";
}

// calculate the distance between to entries
// assumes x and y are same size;
float distance(const fVec& x, const fVec &y, const iVec &features) {
    float distance = 0.0;
    for(int i = 0; i != features.size(); ++i) {
        int c = features.at(i);
        distance += ((y.at(c) - x.at(c)) * (y.at(c) - x.at(c)));
    }
    return sqrt(distance);
}

// input: the dataset and the feature whose mean to calculate
// returns the mean of the feature
float featrMean(const vfVec& data, int feature) {
    float sum = 0.0;
    for (int i = 0; i != data.size(); ++i) {
        sum += data.at(i).at(feature);
    }
    return sum/data.size();
}

// input: the dataset and the feature whose std. deviation to calulate
// returns the std deviation of the feature
float stdDev(const vfVec& data, int featr) {
    float mu = featrMean(data, featr);
    float sum = 0.0;
    for (int i = 0; i != data.size(); ++i) {
        sum +=
         ((data.at(i).at(featr)-mu) * (data.at(i).at(featr)-mu) / data.size());
    }
    return sqrt(sum);
}

// input: the dataset
// alters the datset, with the normalized values, except for the class column
void zNormalize(vfVec& data) {
    for (int i = 0; i != data.size(); ++i) {
        for (int j = 1; j != data.at(i).size(); ++j) {
            data.at(i).at(j) -= featrMean(data, j);
            data.at(i).at(j) /= stdDev(data, j);
        }
    }
}

// hide this data.
fVec validationData(const vfVec& data, int index) {
    fVec validation(data.at(index));
    validation.at(0) = -1; // hide the class
    return validation;
}

// copy the data, minus the validation data
vfVec trainingData(const vfVec& data, int index) {
    vfVec training;
    for (int i = 0; i != data.size(); ++i) {
        if (i==index) continue;
        training.push_back(data.at(i));
    }
    return training;
}

void featureSearch(const vfVec& data) {
    iVec features;
    iVec tempFeatures;
    vector<bstFeats> best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.0;          // 50% default accuracy
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAcc;
        for (int j = 1; j != data.at(0).size(); ++j) {
            if (find(features.begin(), features.end(), j) == features.end()) {
                tempFeatures = features;
                tempFeatures.push_back(j);
                cout << "Using features: ";
                print(tempFeatures);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                cout << setprecision(4)
                    << ", accuracy is: " << accuracy*100 << "%\n";
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
        // print out information on accuracy for the current level
        if (best.at(0).accuracy > bestAccuracy) {
            cout << "\n(WARNING: Accuracy has decreased. "
            "Continuing search in case of local maxima.)\n";
        }
        else {
            cout << "\nFeature set {";
            print(best.at(0).features);
            cout << "} was best, accuracy is " << bestAccuracy*100 << "%\n\n";
        }
    }
    cout << "\nbest\n";
    sort(best.begin(),best.end(), cmpFeatures); // move best accuracy to front
    cout << best.at(0).accuracy * 100 << "% {";
    print(best.at(0).features);
    cout << endl;
}

float leaveOneOutCrossValidation(const vfVec& data, const iVec& features) {
    float numCorrect=0;
    for (int index = 0; index != data.size(); ++index) {
        fVec validation = validationData(data,index);
        vfVec training = trainingData(data,index);
        int k = chooseBestK(data, training, validation, features, index);
        classify(training, validation, features, k);
        if (accurate(data, validation, index)) ++numCorrect;
    }
    return numCorrect/data.size();
}

int chooseBestK(const vfVec &originalData, const vfVec &training,
                const fVec &validation, const iVec &features, int index) {
    int numberPointstoCheck = static_cast<int>(originalData.size())/3;
    // k can't exceed number of data points
    for (int k = 1; k < numberPointstoCheck; k = k+2) {
        fVec tempValidation = validation;
        classify(training, tempValidation, features, k);
        if (accurate(originalData, tempValidation, index)){
            return k;
        }
    }
    return 1;      // ran out of neighbors. Use 1 nearest neighbor
}

// returns the accurary of the tested data.
// using leave one out so it's either correct or incorrect
bool accurate(const vfVec& originalData, const fVec& validation, int index){
    return (originalData.at(index).at(0) == validation.at(0));
}

void classify(const vfVec &training, fVec &validation,
              const iVec &features, int k) {
    validation.at(0) = knn(training, validation, features, k);
}

// returns class of k-nearest neighbors
int knn(const vfVec& training, const fVec& validation,
        const iVec& features, int k) {
    fiMap distances; // holds classes in order of increasing distance
    // find the nearest points in the training data
    for (int i = 0; i != training.size(); ++i) {
        float tempDistance = distance(training.at(i), validation, features);
        distances[tempDistance] = training.at(i).at(0);
    }
    return vote(distances,k); // pick the k-val that was seen the most out of k
}

int vote(const fiMap& distances, int k) {
    int index = 0;
    // count the votes for each class
    map<int, int> classesSeen; // hold the # of times a class is seen
    for (fiMap::const_iterator i=distances.begin(); i != distances.end(); ++i) {
        if (index++ == k) break;
        ++classesSeen[i->second];
    }
    
    // find out which class had the most votes
    int max = -1;
    int classificationIs = -1;
    for (map<int,int>::iterator i = classesSeen.begin(); i != classesSeen.end();
         ++i) {
        if (i->second > max ) {
            max = i->second;
            classificationIs = i->first;
        }
    }
    return classificationIs;
}
