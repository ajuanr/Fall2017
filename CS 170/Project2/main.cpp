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

struct bstFeats;                    // forwward declaration

typedef vector<float> fVec;         // holds the features
typedef vector<fVec> vfVec;         // holds all the rows
typedef vector<int> iVec;           // holds which features to look at
typedef vector<bstFeats> bVec;      // for storing the trials
typedef map<float, int> fiMap;
typedef map<int,int> iMap;

struct bstFeats{
    bstFeats():accuracy(0.5){ }
    bstFeats(float a, iVec f):accuracy(a), features(f) { }
    float accuracy;
    iVec features;
};

vfVec readData();
fVec parseLine(const string);
void print(const vfVec&);
void print(const iVec&);
int chooseBestK(const vfVec&, const vfVec&, const fVec&, const iVec&, int);
int knn(const vfVec&, const fVec&,const iVec&, int);
float distance(const fVec&, const fVec&, const iVec&);
fVec validationData(const vfVec&, int);
vfVec trainingData(const vfVec&, int);
void classify(const vfVec&, fVec&, const iVec&, int);
// normalizing stuff
float featureMean(const vfVec&, int);
float stdDev(const vfVec&, int);
void zNormalize(vfVec&);
bool accurate(const vfVec&, const fVec&, int);
int vote(const fiMap&, int);
bool cmpFeatures(const bstFeats &a,const bstFeats &b)
    {return a.accuracy>b.accuracy;}

//search stuff
float leaveOneOutCrossValidation(const vfVec&, const iVec&);
bstFeats* forwardSelectionDemo(const vfVec&);
bstFeats* forwardSelection(const vfVec&);
bstFeats* backwardElimDemo(const vfVec&);
bstFeats* backwardElim(const vfVec&);
iVec allFeatures(const vfVec&);
vfVec randomData(const vfVec&);
void repeatedtrialResults(const vector<bstFeats>&);

// output stuff
void introduction(vfVec&);
void resultsInfo(const vfVec&);

void testNormalize(vfVec&);

/*****************************************************************************/
int main(int argc, const char * argv[]) {
    vfVec data = readData();
    if (!data.empty()) {
        testNormalize(data);
//        vfVec dataCopy = data;
//        normalize(data);
//        print(data);
//        cout << "\n\n\n";
//        zNormalize(dataCopy);
//        print(dataCopy);
//        introduction(data);
//        resultsInfo(data);
//        resultsInfo(data);
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
    const string fileName = "wine.txt";
//    const string fileName = "DataUserModeling.txt";
    ifstream input;
    input.open(fileName, ifstream::in);
    vfVec output;
    if (!input.is_open()) {
        cout << "File failed to open\n";
        return output;
    }
    string line;
    while (getline (input,line) ) {
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
float featureMean(const vfVec& data, int feature) {
    float sum = 0.0;
    for (int i = 0; i != data.size(); ++i) {
        sum += data.at(i).at(feature);
    }
    return sum/data.size();
}

// input: the dataset and the feature whose std. deviation to calulate
// returns the std deviation of the feature
float stdDev(const vfVec& data, int feature) {
    float mu = featureMean(data, feature);
    float sum = 0.0;
    for (int i = 0; i != data.size(); ++i) {
        sum +=
         ((data.at(i).at(feature)-mu)*(data.at(i).at(feature)-mu) / data.size());
    }
    return sqrt(sum);
}

// input: the dataset
// alters the datset, with the normalized values, except for the class column
void zNormalize(vfVec &data) {
    vfVec dataCopy = data;
    for (int feature = 1; feature != data.at(0).size(); ++feature) {
        float mean = featureMean(dataCopy, feature);
        float stdDeviation = stdDev(dataCopy, feature);
        for (int instance = 0; instance != data.size(); ++instance) {
            data.at(instance).at(feature) -= mean;
            data.at(instance).at(feature) /= stdDeviation;
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
        if (i == index) continue;
        training.push_back(data.at(i));
    }
    return training;
}

bstFeats* forwardSelectionDemo(const vfVec& data) {
    iVec features;
    iVec tempFeatures;
    vector<bstFeats> best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.5;          // 50% default accuracy
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
            cout << "\nFeature set ";
            print(best.at(0).features);
            cout << " was best, accuracy is " << bestAccuracy << "\n\n";
        }
    }
    sort(best.begin(),best.end(), cmpFeatures); // move best accuracy to front
    bstFeats *bestFeature = new bstFeats(best.at(0).accuracy, best.at(0).features);
    return bestFeature;
}

bstFeats* forwardSelection(const vfVec& data) {
    iVec features;
    iVec tempFeatures;
    vector<bstFeats> best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.5;          // 50% default accuracy
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAcc;
        for (int j = 1; j != data.at(0).size(); ++j) {
            if (find(features.begin(), features.end(), j) == features.end()) {
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
    }
    sort(best.begin(),best.end(), cmpFeatures); // move best accuracy to front
    bstFeats *bestFeature = new bstFeats(best.at(0).accuracy, best.at(0).features);
    return bestFeature;
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

bstFeats* backwardElimDemo(const vfVec& data) {
    iVec features = allFeatures(data);
    iVec tempFeatures = features;
    vector<bstFeats> best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.0;
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAcc;
        for (int j = 0; j != features.size(); ++j) {
            if (find(features.begin(), features.end(), j) != features.end()) {
                tempFeatures = features;
                tempFeatures.erase(tempFeatures.begin()+j);
                cout << "Using features: ";
                print(tempFeatures);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                cout << setprecision(4)
                << ", accuracy is: " << accuracy << "\n";
                cout << "best accuracy is; " << bestAccuracy << endl;
                if ( accuracy > bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        if (features.size() == 1) --bestAtThisLevel;
        features.erase(features.begin() + (bestAtThisLevel));
        bstFeats temp(bestAccuracy, features);
        best.push_back(temp);
        sort(best.begin(),best.end(), cmpFeatures);
        // print out information on accuracy for the current level
        if (best.at(0).accuracy > bestAccuracy) {
            cout << "\n(WARNING: Accuracy has decreased. "
            "Continuing search in case of local maxima.)\n";
        }
        else {
            cout << "\nFeature set ";
            print(best.at(0).features);
            cout << " was best, accuracy is " << bestAccuracy << "\n\n";
        }
    }
    cout << "\nbest\n";
    sort(best.begin(),best.end(), cmpFeatures); // move best accuracy to front
    bstFeats *bestFeature = new bstFeats(best.at(0).accuracy, best.at(0).features);
    return bestFeature;
}

bstFeats* backwardElim(const vfVec& data) {
    iVec features = allFeatures(data);
    iVec tempFeatures = features;
    vector<bstFeats> best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.0;
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAcc;
        for (int j = 0; j != features.size(); ++j) {
            if (find(features.begin(), features.end(), j) != features.end()) {
                tempFeatures = features;
                tempFeatures.erase(tempFeatures.begin()+j);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                if ( accuracy > bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        if (features.size() == 1) --bestAtThisLevel; // for the last feature
        features.erase(features.begin() + (bestAtThisLevel));
        bstFeats temp(bestAccuracy, features);
        best.push_back(temp);
        sort(best.begin(),best.end(), cmpFeatures);
    }
    sort(best.begin(),best.end(), cmpFeatures); // move best accuracy to front
    bstFeats *bestFeature = new bstFeats(best.at(0).accuracy, best.at(0).features);
    return bestFeature;
}

// returns a vector a value for every feature in the data
// used for backward elimination;
iVec allFeatures(const vfVec& data) {
    iVec features;
    for (int i = 1; i != data.at(0).size(); ++i) {
        features.push_back(i);
    }
    return features;
}

vfVec randomData(const vfVec &data) {
    srand(static_cast<unsigned int>(time(0)));
    vfVec newData;
    iVec skipThese;
    float percentToSkip = .05;
    int numToSkip = data.size() * percentToSkip;
    for (int i = 0; i != numToSkip; ++i) {
        int temp = rand()%100;
        if (find(skipThese.begin(), skipThese.end(),temp) == skipThese.end()) {
            skipThese.push_back(temp);
        }
        else --i; // redo this iteration since we already saw this temp value
    }
    for (int i = 0; i != data.size(); ++i) {
        if(find(skipThese.begin(), skipThese.end(),i) == skipThese.end()) {
            // not skipping this i
            newData.push_back(data.at(i));
        }
    }
    return newData;
}

void repeatedtrialResults(const bVec& trials) {
    iMap results;
    for (int i = 0; i != trials.size(); ++i) {
        for(int j = 0; j != trials.at(i).features.size(); ++j) {
            ++results[trials.at(i).features.at(j)];
        }
    }
    for (iMap::iterator i = results.begin(); i != results.end(); ++i) {
        cout << setw(3) << i->first << " was seen: "
             << setw(2) << i->second << " times ";
        for (int k = 0; k != i->second; ++k) {
            cout << "#";
        }
        cout << endl;
    }
}

void introduction(vfVec &data) {
    cout << "Please wait while I normalize the data...   ";
//    zNormalize(data);
    zNormalize(data);
    cout << "Done\n\n";
    cout << "This dataset has " << data.at(0).size()-1 <<
    " features (not including the class attribute) with "
    << data.size() << " instances.\n\n";
    cout << "Beginning Search\n\n";
}

void resultsInfo(const vfVec &data) {
    vector<bstFeats> results;
    int numTrials = 10;
    for (int i =0; i != numTrials; ++i) {
        vfVec newData = randomData(data);
        bstFeats *best = forwardSelection(newData);
        cout << setw(5) << setprecision(5) << best->accuracy  << " ";
        print(best->features);
        cout << endl;
        results.push_back(*best);
    }
    cout << endl << endl;
    cout << "Number of trials: " << numTrials << endl;
    repeatedtrialResults(results);
}

void testNormalize(vfVec &data) {
    //7, 4, 12, 5, 4, 10, 9, 6, 9, 4
    fVec data1 = {0,9};
    fVec data2 = {0,2};
    fVec data3 = {0,5};
    fVec data4 = {0,4};
    fVec data5 = {0,12};
    fVec data6 = {0,7};
    fVec data7 = {0,8};
    fVec data8 = {0,11};
    fVec data9 = {0,9};
    fVec data10 = {0,3};
    fVec data11 = {0,7};
    fVec data12 = {0,4};
    fVec data13 = {0,12};
    fVec data14 = {0,5};
    fVec data15 = {0,4};
    fVec data16 = {0,10};
    fVec data17 = {0,9};
    fVec data18 = {0,6};
    fVec data19 = {0,9};
    fVec data20 = {0,4};
    vfVec d;
    d.push_back(data1);
    d.push_back(data2);
    d.push_back(data3);
    d.push_back(data4);
    d.push_back(data5);
    d.push_back(data6);
    d.push_back(data7);
    d.push_back(data8);
    d.push_back(data9);
    d.push_back(data10);
    d.push_back(data11);
    d.push_back(data12);
    d.push_back(data13);
    d.push_back(data14);
    d.push_back(data15);
    d.push_back(data16);
    d.push_back(data17);
    d.push_back(data18);
    d.push_back(data19);
    d.push_back(data20);
    print(d); cout << endl;
    cout << featureMean(d, 1) << endl;
    cout << stdDev(d, 1) << endl;
    zNormalize(d);
    print(d);
    cout << endl;
    
    
}
