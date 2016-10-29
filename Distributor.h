//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H
#define POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H


#include <climits>
#include "Tape.h"
#include "FibonacciGenerator.h"

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

    FibonacciGenerator fibonacciGenerator;


public:
    Distributor(unsigned int bufferSize_,
                const std::string& inputPath,
                const std::string& aPath,
                const std::string& bPath):
            inputTape(inputPath,"INPUT",bufferSize_,true),
            aTape(aPath, "A",bufferSize_),
            bTape(bPath, "B",bufferSize_),
            readBuffer(bufferSize_)
    {
        seriesOnA = 0;
        seriesOnB = 0;
        currentTape = &aTape;
        bufferSize = bufferSize_;

        fibonacciGenerator = FibonacciGenerator();
    }

    void count_new_series() {
        int series;
        if (currentTape == &aTape) {
            series = ++seriesOnA;
        } else if (currentTape == &bTape) {
            series = ++seriesOnB;
        }
        std::cout << "*  zliczono "
                  << series << "/" << fibonacciGenerator.limit()
                  << " serii na " << currentTape->getName()
                  << ", ostatni = " << currentTape->last_from_buffer().getVolume()
                  << std::endl;

    }

    bool fib_limit_reached() {
        unsigned int series, limit;
        if (currentTape == &aTape) {
            series = seriesOnA;
        } else if (currentTape == &bTape) {
            series = seriesOnB;
        }
        limit = fibonacciGenerator.limit();

        if (series < limit)
            return false;
        else {
            std::cout << "osiagnieto max serii=" << limit << std::endl;
            return true;
        }
    }

    void switch_current_tape() {
        currentTape = (currentTape == &aTape) ? &bTape : &aTape;
    }

    void decrease_series_counter() {
        unsigned int series;
        if (currentTape == &aTape) {
            series = --seriesOnA;
        } else if (currentTape == &bTape) {
            series = --seriesOnB;
        }
        std::cout << "kontynuacja na tasmie A nadal jest "
                  << series << " serii" << std::endl;
    }

    void distribute(){
        Cone last = Cone(INT_MIN, INT_MIN);

        //inputTape.getStream().close();
        //inputTape.getStream().open(inputTape.getPath().c_str(), std::ios::in | std::ios::binary);

        while(inputTape.getStream().read(reinterpret_cast<char *>(readBuffer.data()), sizeof(Cone) * bufferSize)){
            for(unsigned int readBufferIndex = 0; readBufferIndex<bufferSize;readBufferIndex++){
                do{
                    last = currentTape->last_from_buffer();

                    //  next element in the series(we are prolonging series length)
                    if(last <= readBuffer.at(readBufferIndex)){
                        currentTape->insert_element_into_tape_buffer(readBuffer.at(readBufferIndex));
                        elementInserted = true;
                    }else{
                        //  we can't put element in ascending order anymore
                        count_new_series();

                        //  there is a possibility to create a new series on current tape, up to fib limit
                        if(fib_limit_reached()){
                            switch_current_tape();
                            fibonacciGenerator.next();

                            //  maybe we can continue series that we found on freshly switched tape
                            if(currentTape->last_from_buffer() <= readBuffer.at(readBufferIndex) && !bTape.getBuffer().empty()){
                                //  continue previous series
                                decrease_series_counter();
                            }

                            currentTape->insert_element_into_tape_buffer(readBuffer.at(readBufferIndex));
                            elementInserted= true;
                        }else{
                            //  we cannot put more new series on current tape
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
