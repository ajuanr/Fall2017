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
#include <sstream>          // to parse a line of text
#include <vector>                   // to hold the data.

using namespace std;

struct Feature;                    // forward declaration

typedef vector<float> fVec;         // holds the features
typedef vector<fVec> vfVec;         // holds all the rows
typedef vector<int> iVec;           // holds which features to look at
typedef vector<iVec> viVec;
typedef vector<Feature> bVec;      // for storing the trials
typedef map<float, int> fiMap;

typedef void(*searchFunc)(const vfVec&);
typedef Feature(*searchNoInfo)(const vfVec&);

struct Feature{
    Feature():accuracy(0.5){ }
    Feature(float a, iVec f):accuracy(a), features(f) { }
    float accuracy;
    iVec features;
};
// input stuff
vfVec readData(const string);
fVec parseLine(const string);
void print(const vfVec&);
void print(const iVec&);
// knn stuff
int knn(const vfVec&, const fVec&,const iVec&, int);
float distance(const fVec&, const fVec&, const iVec&);
fVec validationData(const vfVec&, int);
vfVec trainingData(const vfVec&, int);
void classify(const vfVec&, fVec&, const iVec&, int);
// normalizing stuff
float featureMean(const vfVec&, int);
float stdDev(const vfVec&, int);
void zNormalize(vfVec&);
// search stuff
bool accurate(const vfVec&, const fVec&, int);
bool compare(const Feature &a,const Feature &b)
    {return a.accuracy>b.accuracy;}
float leaveOneOutCrossValidation(const vfVec&, const iVec&);
void forwardSelectionDemo(const vfVec&);
void backwardElimDemo(const vfVec&);
iVec allFeatures(const vfVec&);
// my search algorithm
iVec featureBitVector(int);
viVec initPopulation(int size);
iVec crossOver(const iVec&, const iVec&);
void crossoverParents(viVec&);
void mutate(iVec&);
Feature evaluate(const vfVec&, const iVec&);
void evolve(const vfVec&);
void printBitVector(const iVec&);
// output stuff
string promptFileName();
searchFunc promptSearchFunction();
void startSearch(vfVec&);

/*****************************************************************************/
int main(int argc, const char * argv[]) {
    vfVec data = readData(promptFileName());
    if (!data.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        startSearch(data);

    }
    else
        cout << "There's no data\n";
    return 0;
}
/*****************************************************************************/

string promptFileName() {
    cout << "Welcome to Juan Ruiz's feature selection algorithm\n"
    "Type in the name of the file to test: ";
    string fileName;
    cin >> fileName;
    return fileName;
}

searchFunc promptSearchFunction() {
    cout << "Type the number of the algorithm you want to run\n"
    "1. Forward Selection\n"
    "2. Backward Elimination\n"
    "3. Juan's Special Algorithm\n"
    "        ";
    int choice;
    cin >> choice;
    switch (choice) {
        case(1): return forwardSelectionDemo;
        case(2): return backwardElimDemo;
        case(3): return evolve;
        default:return forwardSelectionDemo;
    }
}

void startSearch(vfVec &data) {
    searchFunc searchFunction = promptSearchFunction();
    cout << "This dataset has " << data.at(0).size()-1
    << " features (not including the class attribute) with "
    << data.size() << " instances.\n\n";
    cout << "Please wait while I normalize the data...   ";
    zNormalize(data);
    cout << "Done\n\n"
    << "Beginning Search\n\n";
    searchFunction(data);
}

// Read the data and return a vector containing the data
vfVec readData(const string fileName){
    cout << "\nUsing: " << fileName << endl << endl;
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
    if (features.empty()) {
        cout << "{ }";
        return;
    }
    cout << "{" << features.at(0);
    for (int i = 1; i != features.size(); ++i) {
        cout << " " << features.at(i);
    }
    cout << "}";
}

void printBitVector(const iVec &bits) {
    cout << "{";
    if (bits.at(0) == 1) cout << 1;
    for (int i = 1; i != bits.size(); ++i) {
        if (bits.at(i) == 1) {
            cout << " " << i+1;
        }
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
    for (int feature = 1; feature != data.at(0).size(); ++feature) {
        float mean = featureMean(data, feature);
        float stdDeviation = stdDev(data, feature);
        for (int instance = 0; instance != data.size(); ++instance) {
            float x = data.at(instance).at(feature);
            data.at(instance).at(feature) = (x - mean)/stdDeviation;
        }
    }
}

// hide this data.
fVec validationData(const vfVec& data, int instance) {
    fVec validation(data.at(instance));
    validation.at(0) = -1; // hide the class
    return validation;
}

// copy the data, minus the validation data
vfVec trainingData(const vfVec& data, int instance) {
    vfVec training;
    for (int i = 0; i != data.size(); ++i) {
        if (i == instance) continue;    // leave out the instance
        training.push_back(data.at(i));
    }
    return training;
}

void forwardSelectionDemo(const vfVec& data) {
    iVec currentSet;
    Feature best(0, currentSet);
    for (int i = 1; i != data.at(0).size(); ++i) {
        int featureToAddAtThisLevel = i;
        float bestAccuracySoFar = 0.0;
        for (int j = 1; j != data.at(0).size(); ++j) {
            iVec tempFeatures = currentSet;
            if (find(currentSet.begin(), currentSet.end(), j)
                == currentSet.end()) {
                tempFeatures.push_back(j);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                cout << "Using features: " << setw(5) << accuracy << " ";
                print(tempFeatures); cout << endl;
                if (accuracy > bestAccuracySoFar) {
                    bestAccuracySoFar = accuracy;
                    featureToAddAtThisLevel = j;
                }
            }
        }
        currentSet.push_back(featureToAddAtThisLevel);
        if (bestAccuracySoFar > best.accuracy) {
            best = Feature(bestAccuracySoFar, currentSet);
            cout << "\nFeature set ";
            print(best.features);
            cout << " was best, accuracy is " << bestAccuracySoFar << "\n\n";
        }
        else {
            cout << "\n(WARNING: Accuracy has decreased. "
            "Continuing search in case of local maxima.)\n\n";
        }
    }
    cout << best.accuracy<< " "; print(best.features); cout << endl;
}

void backwardElimDemo(const vfVec& data) {
    iVec features = allFeatures(data);
    iVec tempFeatures = features;
    Feature best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.5;
    cout << "accuracy with all features: "
    << leaveOneOutCrossValidation(data, features) << endl;
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAcc;
        for (int j = 0; j != features.size(); ++j) {
            if (find(features.begin(), features.end(), features.at(j))
                != features.end()) {
                tempFeatures = features;
                tempFeatures.erase(tempFeatures.begin()+j);
                cout << "Using features: ";
                print(tempFeatures);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                cout << setprecision(4)
                << ", accuracy is: " << accuracy << "\n";
                if ( accuracy > bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        features.erase(features.begin() + (bestAtThisLevel));
        Feature temp(bestAccuracy, features);
        if (temp.accuracy > best.accuracy) {
            best.accuracy = temp.accuracy;
            best.features = temp.features;
            cout << "\nFeature set ";
            print(best.features);
            cout << " was best, accuracy is " << bestAccuracy << "\n\n";
        }
        // print out information on accuracy for the current level
        else {
            cout << "\n(WARNING: Accuracy has decreased. "
            "Continuing search in case of local maxima.)\n";
        }
    }
    cout << "\n\nBest is: " << best.accuracy << " ";
    print(best.features); cout << endl;
}

float leaveOneOutCrossValidation(const vfVec& data, const iVec& features) {
    int numCorrect=0;
    for (int instance = 0; instance != data.size(); ++instance) {
        fVec validation = validationData(data,instance);
        vfVec training = trainingData(data,instance);
        classify(training, validation, features, 1);
        if (accurate(data, validation, instance)) ++numCorrect;
    }
    return static_cast<float>(numCorrect)/data.size();
}

// returns the accurary of the tested data.
// using leave one out so it's either correct or incorrect
bool accurate(const vfVec& originalData, const fVec& validation, int instance){
    return (originalData.at(instance).at(0) == validation.at(0));
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
    return distances.begin()->second;
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

iVec featureBitVector(int numFeatures) {
    iVec output;
    for (int i = 0; i != numFeatures; ++i) {
        output.push_back(rand()%2);
    }
    return output;
}

viVec initPopulation(int numFeatures) {
    viVec startingPopulation;
    float percentToUse = 0.3;
    int populationSize = numFeatures * percentToUse;
    if ((populationSize % 2) == 1) ++populationSize; // no odd populations
    for (int i = 0; i != populationSize; ++i) {
        iVec parent = featureBitVector(numFeatures);
        startingPopulation.push_back(parent);
    }
    return startingPopulation;
}

iVec crossOver(const iVec& a, const iVec& b) {
    iVec child;
    int maxCrossOverPoint = a.size() * .7;
    int minCrossOverPoint = a.size() * .3;
    int crossOverPoint = (rand() % maxCrossOverPoint)+minCrossOverPoint;
    for (int i = 0; i != a.size(); ++i) {
        (i<crossOverPoint)?child.push_back(a.at(i)) : child.push_back(b.at(i));
    }
    return child;
}

void crossoverParents(viVec& parents) {
    // pair parents randomly
    iVec pairOder;
    for (int i = 0; i != parents.size(); ++i) {
        int temp = rand() % parents.size();
        if (find(pairOder.begin(), pairOder.end(), temp) != pairOder.end()) {
            --i; // redo this iteration
        }
        else { pairOder.push_back(temp); }
    }
    // perform the crossover on the pairs
    int numParents = static_cast<int>(parents.size());
    int i = 0;
    while (i < numParents) {
        // add the child of the crossover to parents vector
        parents.push_back(crossOver(parents.at(i), parents.at(i+1)));
        i += 2; // move on to next pair
    }
}

void mutate(iVec& individual) {
    if (rand()%individual.size() == 0) {
        int mutationPoint = rand() % individual.size();
        if (individual.at(mutationPoint) == 0)
            individual.at(mutationPoint) = 1;
        else individual.at(mutationPoint) = 0;
    }
}

Feature evaluate(const vfVec& data, const iVec& individual) {
    // convert features used to a format useable by leaveOneOutCrossValidation
    iVec featuresVector;
    for (int i = 0; i != individual.size(); ++i) {
        if (individual.at(i) == 1) {
            featuresVector.push_back(i+1);
        }
    }
    float acc = leaveOneOutCrossValidation(data, featuresVector);
    return Feature(acc, individual);
}

void evolve(const vfVec &data) {
    // initialize parents
    viVec parents = initPopulation(static_cast<int>(data.at(0).size())-1);
    bVec population;
    int maxGenerations = 100;
    // want to keep pop. through each generation at this size
    int originalNumParents = static_cast<int>(parents.size());
    for (int generation = 0; generation != maxGenerations; ++generation) {
        // clear the old population, if any
        population.clear();
        // perform crossover
        crossoverParents(parents);
        // possibly mutate some individuals
        for (int i = 0; i != parents.size(); ++i) {
            mutate(parents.at(i));
        }
        // evalute the population
        for ( int i = 0; i != parents.size(); ++i) {
            population.push_back(evaluate(data, parents.at(i)));
        }
        sort(population.begin(), population.end(), compare);
        // select parents for next round
        int numCopy=static_cast<int>(population.size())-(originalNumParents/2);
        parents.clear();
        for (int i = 0; i != numCopy; ++i) {
            parents.push_back(population.at(i).features);
        }
        cout << " best for generation: " << generation << " was: "
        << population.at(0).accuracy << "  ";
        printBitVector(population.at(0).features); cout << endl;
    }
    cout << "\n\nbest was: " << population.at(0).accuracy << "  ";
    print(population.at(0).features); cout << endl;
    printBitVector(population.at(0).features); cout << endl;
}
