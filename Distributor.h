//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H
#define POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H


#include <climits>
#include "Tape.h"
#define A true
#define B false

class Distributor {
    Tape inputTape;
    Tape aTape;
    Tape bTape;

    Tape* currentTape;
    std::vector<Cone> readBuffer;

    unsigned int bufferSize;
    unsigned int seriesOnA;
    unsigned int seriesOnB;
    bool elementInserted;


public:
    Distributor(unsigned int bufferSize_,
                const std::string& inputPath,
                const std::string& aPath,
                const std::string& bPath):
            inputTape(inputPath,"INPUT",bufferSize_,true),
            aTape(aPath, "A",bufferSize_),
            bTape(bPath, "B",bufferSize_)
    {
        seriesOnA = 0;
        seriesOnB = 0;
        currentTape = &aTape;
        bufferSize = bufferSize_;
        readBuffer.reserve(bufferSize);
    }

    void distribute(){
        Cone last = Cone(INT_MIN, INT_MIN);

        while(inputTape.getStream().read(reinterpret_cast<char *>(readBuffer.data()), sizeof(Cone) * bufferSize)){
            for(unsigned int readBufferIndex = 0; readBufferIndex<bufferSize;readBufferIndex++){
                do{
                    last = last_from_current_buffer();

                    //  next element in the series(we are prolonging series length)
                    if(last <= readBuffer.at(readBufferIndex)){
                        currentTape->insert_element_into_tape_buffer();
                        elementInserted = true;
                    }else{
                        //  we can't put element in ascending order anymore
                        count_new_series();

                        //  we cannot put more new series on current tape
                        if(!fib_limit_reached()){
                            switch_current_tape();
                            fibonacciGenerator.next();

                            //  maybe we can continue series that we found on freshly switched tape
                            if(last_from_current_buffer() <= readBuffer.at(readBufferIndex) && !bTape.getBuffer().empty()){
                                //  continue previous series
                            }

                            currentTape->insert_element_into_tape_buffer(readBuffer.at(readBufferIndex));
                            elementInserted= true;
                        }else{
                            //  there is a possibility to create a new series on current tape, up to fib limit
                            currentTape->insert_element_into_tape_buffer(readBuffer.at(readBufferIndex));
                            elementInserted = true;
                        }
                    }
                }while(!elementInserted);
            }
        }
    }

};


#endif //POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H
