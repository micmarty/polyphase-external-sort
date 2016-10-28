//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H
#define POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H


#include "Tape.h"

class Distributor {

    Tape inputTape;
    Tape aTape;
    Tape bTape;

    unsigned int seriesOnA;
    unsigned int seriesOnB;

public:
    Distributor(const std::string& inputPath,const std::string& aPath,const std::string& bPath):inputTape(inputPath,"INPUT",true),aTape(aPath, "A"),bTape(bPath, "B"){
        seriesOnA = 0;
        seriesOnB = 0;
    }
    void distribute(){

    }

};


#endif //POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H
