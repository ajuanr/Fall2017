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
typedef vector<iVec> viVec;
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
bool cmpFeatures(const bstFeats &a,const bstFeats &b)
{return a.accuracy>b.accuracy;}
//search stuff
float leaveOneOutCrossValidation(const vfVec&, const iVec&);
void forwardSelectionDemo(const vfVec&);
bstFeats forwardSelection(const vfVec&);
void backwardElimDemo(const vfVec&);
bstFeats backwardElim(const vfVec&);
iVec allFeatures(const vfVec&);
vfVec randomData(const vfVec&, float);
void repeatedtrialResults(const vector<bstFeats>&);
// my search algorithm
iVec featureBitVector(int);
viVec initPopulation(int size);
iVec crossOver(const iVec&, const iVec&);
void crossoverParents(viVec&);
void mutate(iVec&);
bstFeats evaluate(const vfVec&, const iVec&);
bstFeats compete(const vfVec&, const iVec&, const iVec&);
void evolve(const vfVec&);
// output stuff
void introduction(vfVec&);
void resultsInfo(const vfVec&);
void testNormalize(vfVec&);

/*****************************************************************************/
int main(int argc, const char * argv[]) {
    vfVec data = readData();
    if (!data.empty()) {
        srand(static_cast<unsigned int>(time(0)));
  
//        iVec a = featureBitVector(data.at(0).size()-1);
//        print(a); cout << endl;
//        iVec b = featureBitVector(10);
//        print(b); cout << endl;
//        print(crossOver(a, b)); cout << endl;
//        mutate(a);
//        print(a); cout << endl;
//        evaluate(data, a);

        
//        viVec parents = initPopulation(10);
//        for (int i = 0; i != parents.size(); ++i) {
//            print(parents.at(i));
//            cout << endl;
//        }
//        cout << endl;
//        crossoverParents(parents);
//        cout << endl;
        
        introduction(data);
        int t1 = time(0);
        evolve(data);
        int t2 = time(0);
        cout << "took: " << t2 - t1 << endl;
        
        t1 = time(0);
        forwardSelectionDemo(data);
        t2 = time(0);
        cout << "took: " << t2 - t1 << endl;
//        cout << endl;
//        backwardElimDemo(data);
//        cout << endl;
//        resultsInfo(data);
    }
    else
        cout << "There's no data\n";
    return 0;
}
/*****************************************************************************/

// Read the data and return a vector containing the data
vfVec readData(){
//    const string fileName = "testData170/CS170Smalltestdata__44.txt";
//        const string fileName = "testData170/CS170BIGtestdata__4.txt";
//        const string fileName = "leaf.txt";
//        const string fileName = "wine.txt";
//        const string fileName = "DataUserModeling.txt";
    const string fileName = "sonar.txt";
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
    iVec features;
    iVec tempFeatures;
    bstFeats best;
    int bestAtThisLevel=-1;
    float defaultAccuracy = 0.5;
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAccuracy;
        for (int j = 1; j != data.at(0).size(); ++j) {
            if (find(features.begin(), features.end(), j) == features.end()) {
                tempFeatures = features;
                tempFeatures.push_back(j);
                cout << "Using features: ";
                print(tempFeatures);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                cout << ", accuracy is: " << accuracy << "\n";
                if ( accuracy > bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        // don't add same 'bestAtThisLevel' more than once
        features.push_back(bestAtThisLevel);
        bstFeats temp(bestAccuracy, features);
        if (temp.accuracy >= best.accuracy) {
            best.accuracy = temp.accuracy;
            best.features = temp.features;
            cout << "\n\nFeature set ";
            print(best.features);
            cout << " was best, accuracy is " << bestAccuracy << "\n\n";
        }
        // print out information on accuracy for the current level
        else {
            cout << "\n(WARNING: Accuracy has decreased. "
            "Continuing search in case of local maxima.)\n";
        }
    }
    cout << "\n\nBest is: ";
    cout << best.accuracy << " ";
    print(best.features);
    cout << endl;
}

bstFeats forwardSelection(const vfVec& data) {
    iVec features;
    iVec tempFeatures;
    bstFeats best;
    int bestAtThisLevel=-1;
    float defaultAccuracy = 0.5;
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAccuracy;
        for (int j = 1; j != data.at(0).size(); ++j) {
            if (find(features.begin(), features.end(), j) == features.end()) {
                tempFeatures = features;
                tempFeatures.push_back(j);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                if ( accuracy >= bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        // don't add same 'bestAtThisLevel' more than once
        features.push_back(bestAtThisLevel);
        bstFeats temp(bestAccuracy, features);
        if (temp.accuracy >= best.accuracy) {
            best.accuracy = temp.accuracy;
            best.features = temp.features;
        }
    }
    return best;
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

void backwardElimDemo(const vfVec& data) {
    iVec features = allFeatures(data);
    iVec tempFeatures = features;
    bstFeats best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.0;
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAcc;
        for (int j = 0; j != features.size(); ++j) {
            if (find(features.begin(), features.end(), features.at(j)) != features.end()) {
                tempFeatures = features;
                tempFeatures.erase(tempFeatures.begin()+j);
                cout << "Using features: ";
                print(tempFeatures);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                cout << setprecision(4)
                << ", accuracy is: " << accuracy << "\n";
                if ( accuracy >= bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        features.erase(features.begin() + (bestAtThisLevel));
        bstFeats temp(bestAccuracy, features);
        if (temp.accuracy >= best.accuracy) {
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
    cout << "\n\nbest\n";
    cout << best.accuracy << " ";
    print(best.features);
    cout << endl;
}

bstFeats backwardElim(const vfVec& data) {
    iVec features = allFeatures(data);
    iVec tempFeatures = features;
    bstFeats best;
    int bestAtThisLevel=-1;
    float defaultAcc = 0.0;
    for (int i = 1; i != data.at(0).size(); ++i) {
        float bestAccuracy =  defaultAcc;
        for (int j = 0; j != features.size(); ++j) {
            if (find(features.begin(), features.end(), features.at(j)) != features.end()) {
                tempFeatures = features;
                tempFeatures.erase(tempFeatures.begin()+j);
                float accuracy = leaveOneOutCrossValidation(data, tempFeatures);
                if ( accuracy >= bestAccuracy) {
                    bestAccuracy = accuracy;
                    bestAtThisLevel = j;
                }
            }
        }
        features.erase(features.begin() + (bestAtThisLevel));
        bstFeats temp(bestAccuracy, features);
        if (temp.accuracy >= best.accuracy) {
            best.accuracy = temp.accuracy;
            best.features = temp.features;
        }
    }
    return best;
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

vfVec randomData(const vfVec &data, float percentToSkip) {
    vfVec newData;
    iVec randomiseData;
    int numToSkip = data.size() * percentToSkip;
    for (int i = 0; i != data.size() - numToSkip; ++i) {
        int temp = rand()%data.size();
        if (find(randomiseData.begin(), randomiseData.end(), temp)
            == randomiseData.end()) {
            randomiseData.push_back(temp);
        }
        else --i;   // repeat this trial
    }
    for (int i = 0; i != randomiseData.size(); ++i) {
        newData.push_back(data.at(randomiseData.at(i)));
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
    for (int i = 0; i != numTrials; ++i) {
        vfVec newData = randomData(data,0.05);
//        bstFeats best = forwardSelection(newData);
                bstFeats best = backwardElim(newData);
        cout << setw(8) << setprecision(5) << right <<
        best.accuracy  << " "; print(best.features);
        cout << endl;
        results.push_back(best);
    }
    cout << endl << endl;
    cout << "Number of trials: " << numTrials << endl;
    repeatedtrialResults(results);
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
    if ((populationSize % 2) == 1) ++populationSize; //don't have odd population
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
        (i<crossOverPoint) ? child.push_back(a.at(i)) : child.push_back(b.at(i));
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

bstFeats evaluate(const vfVec& data, const iVec& individual) {
    // convert features used to a format useable by leaveOneOutCrossValidation
    iVec featuresVector;
    for (int i = 0; i != individual.size(); ++i) {
        if (individual.at(i) == 1) {
            featuresVector.push_back(i+1);
        }
    }
    float acc = leaveOneOutCrossValidation(data, featuresVector);
    return bstFeats(acc, individual);
}

bstFeats compete(const vfVec &data, const iVec &a, const iVec &b) {
    bstFeats newA = evaluate(data, a);
    bstFeats newB = evaluate(data, b);
    return (newA.accuracy > newB.accuracy)? newA : newB;
}

void evolve(const vfVec &data) {
    // initialize population
    viVec parents = initPopulation(static_cast<int>(data.at(0).size())-1);
    bVec population;
    for (int round = 0; round != 100; ++round) {
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
        sort(population.begin(), population.end(), cmpFeatures);
        // create the new parents for next round
        int numCopy = population.size() - 2; // leave out the worst two
        parents.clear();
        for (int i = 0; i != numCopy; ++i) {
            parents.push_back(population.at(i).features);
        }
        cout << "best for round: " << round << " was: " << population.at(0).accuracy << "  ";
        print(population.at(0).features); cout << endl;
    }
    cout << "\n\nbest was: " << population.at(0).accuracy << "  ";
    print(population.at(0).features); cout << endl;
}
