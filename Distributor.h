//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H
#define POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H


#include <climits>
#include <utility>
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

    int bufferSize;
    int seriesOnA;
    int seriesOnB;
    bool elementInserted;

    FibonacciGenerator fibonacciGenerator;


public:
    Distributor(int bufferSize_,
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

    ~Distributor(){

    }

    //  count new series on SPECIFIC tape
    void count_new_series(Tape* tape) {
        int series;
        if (tape == &aTape) {
            series = ++seriesOnA;
        } else if (tape == &bTape) {
            series = ++seriesOnB;
        }
        std::cout << "*  zliczono "
                  << series << "/" << fibonacciGenerator.limit()
                  << " serii na " << tape->getName()
                  << std::endl<<std::endl;
    }

    //  count new series on CURRENT tape
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
        int series, limit;
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
        int series;
        if (currentTape == &aTape) {
            series = --seriesOnA;
        } else if (currentTape == &bTape) {
            series = --seriesOnB;
        }
        std::cout << "kontynuacja na tasmie " << currentTape->getName()<<" nadal jest "
                  << series << " serii" << std::endl;
    }




    void distribute(){
        // initially: show file size, count how many whole buffers we need to read
        // and how big is the last one(not fully filled)
        int fileSize = inputTape.file_size();
        std::cout<< "taśma INPUT zawiera " << fileSize << " bajtów"<<std::endl;
        int recordsInFile = fileSize/sizeof(Cone);// size in records
        int wholeBuffersNumber = recordsInFile / bufferSize;
        int recordsInLastBuffer = recordsInFile - wholeBuffersNumber * bufferSize;


        // begin the proper action
        int buffersAlreadyRead = 0;
        while(buffersAlreadyRead < wholeBuffersNumber){
            LAST_READ:  //  label used only once in last run(smaller chunk at the end of the input file)

            inputTape.getStream().read(reinterpret_cast<char *>(readBuffer.data()), sizeof(Cone) * bufferSize);
            inputTape.readsFromTheDisk++;

            for(int readBufferIndex = 0; readBufferIndex<bufferSize;readBufferIndex++){
                do{
                    Cone last = currentTape->last_from_buffer();

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
            buffersAlreadyRead++;
        }


        //  process last, a non-fully filled buffer
        if(recordsInLastBuffer>0){
            bufferSize = recordsInLastBuffer;   //  set the new chunk size for reading
            recordsInLastBuffer=-1;             //  just as a flag
            goto LAST_READ;
        }
        count_new_series(currentTape);   //  we had to stop at some point, so no matter what, we need to count it as a series
                                    //                 . . . 1 3 | 1 |
                                    //                               ^
                                    //  that's what we need to count

        //  write records that are still in buffer
        aTape.flush_buffer_to_tape();
        bTape.flush_buffer_to_tape();


        display_statistics();
    }

    void display_statistics() {
        std::cout<<"PO DYSTRYBUCJI NA A jest: "<<seriesOnA<<" serii\n";
        std::cout<<"PO DYSTRYBUCJI NA B jest: "<<seriesOnB<<" serii\n\n";
        std::cout<<"ODCZYTY Z TAŚMY INPUT: "<<inputTape.readsFromTheDisk << "\n"
                 <<"ZAPISY NA TAŚMĘ A:     " << aTape.writesToTheDisk<<"\n"
                 <<"ZAPISY NA TAŚMĘ B:     "<<bTape.writesToTheDisk<<"\n\n";
    }



};


#endif //POLYPHASE_EXTERNAL_SORT_DISTRIBUTOR_H
