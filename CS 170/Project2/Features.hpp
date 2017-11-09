//
//  Features.hpp
//  MyMachineLearner
//
//  Created by Juan Ruiz on 11/9/17.
//  Copyright © 2017 Juan Ruiz. All rights reserved.
//

#ifndef Features_hpp
#define Features_hpp

#include <vector>

typedef std::vector<int> vecInt;

class Features {
public:
    Features();
    Features(const vecInt);
private:
    vecInt features;
};

#endif /* Features_hpp */
