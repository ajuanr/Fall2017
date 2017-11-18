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
int numFeats(const string);
void print(const vfVec&);
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
int accuracy(const vfVec&, const fVec&, int);
int vote(const fiMap&, int);
bool myCompare(fVec a, fVec b) { return a.at(0) < b.at(0);}
bool cmpFeatures(const bstFeats &a,const bstFeats &b){return a.accuracy>b.accuracy;}
//search stuff
float leaveOneOutCrossValidation(const vfVec&, const iVec&, int);


/*****************************************************************************/
int main(int argc, const char * argv[]) {
    vfVec data = readData();
    if (!data.empty()) {
        cout << "Normalizing data...";
        zNormalize(data);
        cout << "Done\n\n";
        
        iVec smallFeatures = {1,2,3,4,5,6,7,8,9,10,11};
        
        
        for (int index = 0; index != data.size(); ++index) {
            cout <<"index is: " << index << endl;
            fVec validation = validationData(data, index);
            vfVec training = trainingData(data, index);
            iVec features;
            for (int i =1; i <= validation.size(); ++i) {
                fVec tempValidation = validation;
                features.push_back(i);
                int k =chooseBestK(data, training, tempValidation, features, index);
                classify(training, tempValidation, features, k);
                cout << "accuracy is : " << accuracy(data, tempValidation, index) << endl;
            }
        }
        
        //       print(data);
    }
    else
        cout << "There's no data\n";
    return 0;
}
/*****************************************************************************/

// Read the data and return a vector containing the data
vfVec readData(){
    const string fileName = "CS170Smalltestdata__44.txt";
//    const string fileName = "CS170BIGtestdata__4.txt";
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
void print(const vfVec &data) {
    for (int i = 0; i != data.size(); ++i) {
        for (int j = 0; j != data.at(i).size(); ++j) {
            cout << setprecision(8) << setw(13) << data.at(i).at(j);
        }
        cout << endl;
    }
}

void featureSearch(const vfVec& data) {
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

//input: dataset
// returns the % chance to be right when guessing the class
float defaultAverage(const vfVec& data) {
    map<int, int> numberClassOccurences;
    for (int i = 0; i != data.size(); ++i) {
        ++numberClassOccurences[data.at(i).at(0)];
    }
    float largestClassSize = numberClassOccurences[1]>numberClassOccurences[2]?
    numberClassOccurences[1] : numberClassOccurences[2];
    
    return largestClassSize / data.size();
}

// calculate the distance between to entries
// assumes x and y are same size;
float distance(const fVec& x, const fVec &y, const iVec &features) {
    float distance = 0;
    for(int i = 0; i != features.size(); ++i) {
        distance += ((y.at(i) - x.at(i)) * (y.at(i) - x.at(i)));
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
        sum += ((data.at(i).at(featr)-mu) * (data.at(i).at(featr)-mu) / data.size());
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

// returns the accurary of the tested data
int accuracy(const vfVec& originalData, const fVec& validation, int index){
    return (originalData.at(index).at(0) == validation.at(0));
}

float leaveOneOutCrossValidation(const vfVec& data, const iVec& features,
                                 int k) {
    float acc = -1;                   // accuracy
    int start = 1, end = start+k;
    float numCorrect=0;
    while (end <= data.size()) {
        fVec validation = validationData(data,start);
        vfVec testing = trainingData(data,start);
        
        //        classify(validation, testing, features,k);
        //        acc = accuracy(data, testing, start);
        if (acc == 100) ++numCorrect;
        ++start;
        ++end;
    }
    return numCorrect/data.size()*100.00;
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


// returns class of k-nearest neighbors
int knn(const vfVec& training, const fVec& validation,
        const iVec& features, int k) {
    fiMap distances;
    // find the nearest points in the training data
    for (int i = 0; i != training.size(); ++i) {
        float tempDistance = distance(training.at(i), validation, features);
        distances[tempDistance] = training.at(i).at(0);
    }
    
    // choose best k-val
    
    return vote(distances,k);//distances.begin()->second;
}

int vote(const fiMap& distances, int k) {
    int index = 0;
    int classOne = 0;
    int classTwo = 0;
    for (fiMap::const_iterator i=distances.begin(); i != distances.end(); ++i) {
        if (index++ == k) break;
        if(i->second == 1) ++classOne;
        else ++classTwo;
    }
    return ((classOne > classTwo) ? 1 : 2);
}

// THIS MIGHT BE TOO ACCURATE? NEED TO LOWER SEARCH FOR K?
int chooseBestK(const vfVec &originalData, const vfVec &training,
                const fVec &validation, const iVec &features, int index) {
    // k can't exceed number of data points
    for (int k = 1; k != originalData.size()/3; k = k+2) {
        fVec tempValidation = validation;
        classify(training, tempValidation, features, k);
        if (accuracy(originalData, tempValidation, index))
            return k;
    }
    return 1;      // ran out of features. Use 1 nearest neighbor
}

void classify(const vfVec &training, fVec &validation,
              const iVec &features, int k) {
    validation.at(0) = knn(training, validation, features, k);
    
}

