//
//  main.cpp
//  MyMachineLearner
//
//  Created by Juan Ruiz on 11/4/17.
//  Copyright © 2017 Juan Ruiz. All rights reserved.
//

#include <iostream>
#include <vector>
#include <stdlib.h>

#include "Features.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    vecInt data = {1,0,0,1,0,0,1,1,1,1,0,0,4,0,0,1,1};
    Features *f = new Features(data);
    for (int i = 0; i != f->numFeatures(); ++i) {
        cout << f->getFeature(i) << ", ";
    }
    cout << endl;
    
    return 0;
}
