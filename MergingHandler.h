//
// Created by miczi on 30.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
#define POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H


#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include<cmath>
#include<stdlib.h>
#include <stdlib.h>
#include "Cone.h"
#include "Tape.h"



#define JEST_W_BUFORZE 0
#define TRZEBA_ZALADOWAC_BUFOR 1
#define KONIEC_TASMY -1
#define END_OF_DUMMIES -1

#define A 1
#define B 2
#define C 3

using namespace std;

class MergingHandler {
    Tape cTape;
    Tape aTape;
    Tape bTape;

    // they are pointing to one of: a,b,c tapes
    Tape* shorterTape;
    Tape* longerTape;
    Tape* destinationTape;
    Tape* currentTape;  //points to shorter or longer pointer

    int indexShorter;
    int indexLonger;
    int* currentIndex;//points to one of the above indexes

    bool freshOnShorter;
    bool freshOnLonger;
    bool *freshBufferOnCurrent;


    bool destinationIsFileSorted;
public:
    MergingHandler(int bufferSize_,
                   const std::string& cPath,
                   const std::string& aPath,
                   const std::string& bPath):
            cTape(cPath,"C", bufferSize_),
            aTape(aPath, "A", bufferSize_,true),
            bTape(bPath, "B", bufferSize_,true)
    {
        shorterTape = NULL;
        longerTape = NULL;
        destinationTape = NULL;
        currentTape = NULL;     //points to shorter or longer pointer

        indexShorter = 0;
        indexLonger = 0;
        currentIndex = NULL;    //points to one of the above indexes

        bool freshOnShorter=false;
        bool freshOnLonger=false;
        bool *freshBufferOnCurrent = NULL;
    }

    ~MergingHandler()
    {

    }

    /*
     * return the opposite tape:
     * possible values to return:
     * - longerTape
     * - shorterTape
     * */
    Tape* opposite_tape(){
        if(currentTape == shorterTape){
            return longerTape;
        }else if(currentTape == longerTape){
            return shorterTape;
        }
    }

    /*
     * returns index on the opposite tape
     */
    int* opposite_index(){
        if(currentIndex == &indexShorter)
            return &indexLonger;
        else if(currentIndex == &indexLonger)
            return &indexShorter;
    }

    bool *opposite_fresh() {
        if(freshBufferOnCurrent == &freshOnShorter)
            return &freshOnLonger;
        else if(freshBufferOnCurrent == &freshOnLonger)
            return &freshOnShorter;
    }

    void try_to_load_next_chunk_into_current_tape() {

        std::streampos bytesLeft = currentTape->file_size_and_save_position();
        int recordsLeft = bytesLeft/ sizeof(Cone);
        *currentIndex = 0;  //reset position counter to the beginning of new buffer
        *freshBufferOnCurrent = true;

        //  load full buffer(with original, maximum size given in constructor)
        if(recordsLeft >= currentTape->getBufferSize())
            currentTape->getStream().read(reinterpret_cast<char *>(currentTape->getBuffer().data()), sizeof(Cone) * currentTape->getBuffer().size());
        else{//  load diminished buffer(the last in the tape)
            //  update fields
            currentTape->resizeBuffer(recordsLeft);
            currentTape->getStream().read(reinterpret_cast<char *>(currentTape->getBuffer().data()), sizeof(Cone) * recordsLeft);
        }

        currentTape->incrementReadCounter();

    }

    bool is_data_to_fill_the_buffer() {
        size_t bytesLeft = currentTape->file_size_and_save_position();
        int recordsLeft = bytesLeft/sizeof(Cone);

        //  there's no more buffer to load, END OF TAPE
        if(recordsLeft == 0){
            return false;
        }else{
            return true;
        }
    }


    int neighbour_status() {
        int neigh_index = (*currentIndex) + 1;

        //  neighbour is still inside current buffer
        if(neigh_index < currentTape->getBuffer().size()){
            if(neigh_index == 0)
                std::cout<<"Nie przekroczymy tasmy. Obecny: poprzedni bufor, sasiad: "<<currentTape->getBuffer().at(neigh_index).getVolume()<<std::endl;
            else
                std::cout<<"Nie przekroczymy tasmy. Obecny: "<<currentTape->getBuffer().at(*currentIndex).getVolume()<<", sasiad: "<<currentTape->getBuffer().at(neigh_index).getVolume()<<std::endl;
            return JEST_W_BUFORZE;   //  nie przekroczy bo jest w zasiegu bufora
        }
        //  neighbour is outside current buffer, we need to load full or diminished buffer(last in the tape)
        else{
            if(is_data_to_fill_the_buffer()){
                return TRZEBA_ZALADOWAC_BUFOR;
            }else{
                std::cout<<"Przekroczymy tasme, bo nie ma dalej żadnej serii."<<std::endl;
                return KONIEC_TASMY;
            }
        }
    }


    bool neighbour_comes_from_the_current_series(Cone current) {
        int neigh_index;
        if(*freshBufferOnCurrent){
            neigh_index = 0;
        }
        else
            neigh_index = *currentIndex + 1;


        //  compare current with next
        if(current <= currentTape->getBuffer().at(neigh_index)){
            std::cout<<"Sąsiad należy do serii."<<std::endl;
            return true;
        }
        std::cout<<"Sąsiad należy do nowej serii."<<std::endl;
        return false;
    }

    Cone neighbour_of_current() {
        int neigh_index;
        if(*freshBufferOnCurrent){
            neigh_index = 0;
            *freshBufferOnCurrent = false;
        }
        else
            neigh_index = *currentIndex + 1;

        (*currentIndex) = neigh_index;   //przesuwamy indeks w prawo
        return currentTape->getBuffer().at(neigh_index);
    }



    Cone change_tape() {
        std::cout<<"Zmieniam taśmę."<<std::endl;
        currentTape = opposite_tape();
        currentIndex = opposite_index();
        freshBufferOnCurrent = opposite_fresh();
        return currentTape->getBuffer().at(*currentIndex);
    }

    void set_as_destination(int tape){
        if(tape == A){
            destinationTape = &aTape;
        }else if(tape == B){
            destinationTape = &bTape;
        }else if(tape == C){
            destinationTape = &cTape;
        }
    }

    void reopen_longer_for_reading() {
        longerTape->getStream().close();
        longerTape->getStream().open(longerTape->getPath(),std::ios::in|std::ios::binary);
    }

    void reopen_destination_for_writing() {
        destinationTape->getStream().close();
        destinationTape->getStream().open(destinationTape->getPath(),std::ios::out|std::ios::binary);
    }

    void reopen_shorter_for_reading() {
        shorterTape->getStream().open(shorterTape->getPath(),std::ios::in|std::ios::binary);
        shorterTape->getStream().clear();
    }

    void swap_roles(Tape* nextShorter) {
        Tape* savedDestination = destinationTape;
        destinationTape = shorterTape;
        shorterTape = nextShorter;
        longerTape = savedDestination;
    }

    void run_merging_process(int seriesOnA, int seriesOnB){
        bool endOfBothTapesFound = false;
        int dummiesLeft = 0;

        //initial state
        destinationTape = &cTape;
        shorterTape = (std::min(seriesOnA,seriesOnB) == seriesOnA)?&aTape:&bTape;
        longerTape = (shorterTape==&aTape)?&bTape:&aTape;// choose the one that is left


        if(FibonacciGenerator::is_fib(seriesOnA) && FibonacciGenerator::is_fib(seriesOnB) && seriesOnA != seriesOnB){
            while(true){
                Tape* nextShorter = merge(dummiesLeft, endOfBothTapesFound);
                if(endOfBothTapesFound)
                    break;

                swap_roles(nextShorter);
                reopen_destination_for_writing();
                reopen_longer_for_reading();
                reopen_shorter_for_reading();
            }
            cout<<"SCALANIE ZAKOŃCZONE. TAŚMA POWINNA BYĆ POSORTOWANA"<<endl;

        }else{
            cout<<"NIE FIBONACI"<<endl;
//            FibonacciGenerator generator = FibonacciGenerator();
//            int dummiesLeft;
//
//            if(seriesOnA==seriesOnB || FibonacciGenerator::is_fib(seriesOnA)){
//                dummiesLeft = generator.nextFibFrom(seriesOnA) - seriesOnB;
//            }else{
//                dummiesLeft = generator.nextFibFrom(seriesOnB) - seriesOnA;
//            }
//
//            Tape* nextShorter = merge(dummiesLeft);
//            for(int stage=2;stage<=(1.45*log2(seriesOnA+seriesOnB));stage++){
//                Tape* nextShorter = merge(0);
//
//                swap_roles(nextShorter);
//                reopen_destination_for_writing();
//                reopen_longer_for_reading();
//                reopen_shorter_for_reading();
//            }
        }
    }

    void reset_buffers() {
        //  clear() -> changes size to 0 and removes all the data
        shorterTape->getBuffer().clear();
        longerTape->getBuffer().clear();
        destinationTape->getBuffer().clear();

        //  restore value that may have been changed
        shorterTape->resizeBuffer(shorterTape->originalBufferSize);
        longerTape->resizeBuffer(longerTape->originalBufferSize);
        destinationTape->setBufferSize(destinationTape->originalBufferSize);
    }



    /*  loads one buffer for shorter and longer tape */
    void load_initial_buffers(){
        for(int bufferToFill = 0; bufferToFill< 2; bufferToFill++){
            if(is_data_to_fill_the_buffer()){
                try_to_load_next_chunk_into_current_tape();
                *currentIndex = 0;
                *freshBufferOnCurrent = false;
            }
            change_tape();
        }
    }

    void start_with_shorter_tape() {
        currentIndex = &indexShorter;
        currentTape = shorterTape;
        freshBufferOnCurrent = &freshOnShorter;
    }

    void reactToDummies(int& dummiesLeft, bool& forceRewritingSeries, int& endOfSeriesFoundCounter) {
        if(dummiesLeft > 0){
            forceRewritingSeries = true;
            endOfSeriesFoundCounter = 0;//reset the value during rewriting process, because there may be multiple series to rewrite
            dummiesLeft--;
        }else if(dummiesLeft == 0){
            cout<<"Serie dummy już się skończyły, rusza normalne scalanie z obu taśm\n";
            dummiesLeft = END_OF_DUMMIES;//special flag to display above message only once
        }
    }

    Tape* merge(int& dummiesLeft, bool& endOfBothTapesFound){  //  zwraca tasme na ktorej przerwano scalanie
        start_with_shorter_tape();
        reset_buffers();
        load_initial_buffers();

        Cone elementOnCurrent = currentTape->getBuffer().at(*currentIndex);
        Cone elementOnOther = opposite_tape()->getBuffer().at(*opposite_index());

        bool endOfOneTapeFound = false;
        int seriesMerged = 0;

        while(true){
            freshOnShorter = false;
            freshOnLonger = false;
            int endOfSeriesFoundCounter = 0;    //napotkanoKoniecSerii -> value between 0 .. 2 , 2 means we can start merging new pair of series
            bool forceRewritingSeries = false;  //if there are dummies, we want to merge given amount of series with empty(dummy series)

            //  modify dummies counter, eventually display message if we're done with them
            reactToDummies(dummiesLeft,forceRewritingSeries,endOfSeriesFoundCounter);

            //if on both tapes we've found the beginning of the new series, exit
            while(endOfSeriesFoundCounter != 2){
                cout<<elementOnCurrent.getVolume()<<" <= "<<elementOnOther.getVolume()<<endl;

                if(elementOnCurrent <= elementOnOther || endOfSeriesFoundCounter == 1 || forceRewritingSeries){
                    destinationTape->insert_element_into_tape_buffer(elementOnCurrent);

                    //  JEST_W_BUFORZE || TRZEBA_ZALADOWAC_BUFOR || KONIEC_TASMY
                    int actionBeforeChangingToNeighbour = neighbour_status();

                    if(actionBeforeChangingToNeighbour == JEST_W_BUFORZE || actionBeforeChangingToNeighbour == TRZEBA_ZALADOWAC_BUFOR){
                        if(actionBeforeChangingToNeighbour == TRZEBA_ZALADOWAC_BUFOR){
                            //load and reset currentIndex to 0, freshBufferLoaded to true
                            try_to_load_next_chunk_into_current_tape();//at this point we know that some buffer exist to load

                            cout<<"ZALADOWALEM KOLEJNY BUFOR!:"<<endl;
                            currentTape->display_buffer_content();
                        }

                        //  if currentElement <= its neighbour, then use him as currentElement
                        if(neighbour_comes_from_the_current_series(elementOnCurrent)){
                            elementOnCurrent = neighbour_of_current();
                        }else{
                            //otherwise, we've found the beginning of the new series
                            //so we need to leave current tape and go back to the other one

                            endOfSeriesFoundCounter++;
                            //TODO something with dummies

                            //swap elements if we're out of dummies
                            if(dummiesLeft == END_OF_DUMMIES){
                                elementOnOther = elementOnCurrent;
                                elementOnCurrent = change_tape();
                            }else{
                                elementOnCurrent = neighbour_of_current();
                            }
                        }
                    }else if(actionBeforeChangingToNeighbour == KONIEC_TASMY){
                        if(endOfOneTapeFound){ //we achieved end of both tapes, so we've merged 1 series and 1 series, then exit
                            endOfBothTapesFound = true;
                        }
                        endOfOneTapeFound = true;
                        elementOnOther = elementOnCurrent;
                        elementOnCurrent = change_tape();//extract element we've end up previously
                        endOfSeriesFoundCounter++;
                    }
                }else{//if elemOnCurrent is not <= elemOnOther
                    //we need to swap them
                    elementOnOther = elementOnCurrent;
                    elementOnCurrent = change_tape();
                }

            }

            seriesMerged++;

            if(endOfOneTapeFound){
                destinationTape->flush_buffer_to_tape();
                std::cout<<"natychmiast wychodze " << std::endl;
                cofnijWskaznikiTasm();
                //tu powinienem zwracac obecnie longer, bo on bedzie w przyszlosci shortem
                return longerTape;//bo po napotkaniu konca, zmienilismy tasme! wyjdz z petli for
            }else{
                //if(neighbour_status() != KONIEC_TASMY){
                    if(!(*freshBufferOnCurrent))
                        *currentIndex += 1; //move to the element from the new series
                    elementOnCurrent = currentTape->getBuffer().at(*currentIndex);
                    cout<<endl<<endl<<endl<<"przygotowanie do " <<seriesMerged + 1<< "serii, elementOnCurrent = " << elementOnCurrent.getVolume()<<" ,index = " <<*currentIndex<<endl;
                //}else{
                //    int cosniehalo=0;
               // }
                //if(neighbour_status() != KONIEC_TASMY){
                    if(!*opposite_fresh())
                        *opposite_index() += 1; //move to the element from the new series
                    elementOnOther = opposite_tape()->getBuffer().at(*opposite_index());
                    cout<<"przygotowanie do " <<seriesMerged + 1<< "serii, elementOnOther = " << elementOnOther.getVolume()<<" ,index = " <<*opposite_index()<<endl;
                //}
                //else{
                //    int cosniehalo=0;
                //}
            }
        }
    }

    void cofnijWskaznikiTasm() {
        //DEBUG
        //longerTape->display_buffer_content();
        //shorterTape->display_buffer_content();
        int potrzebowalElementow = indexLonger + 1;

        if(freshOnLonger)
            potrzebowalElementow = 0;

        int doCofniecia = (longerTape->getBuffer().size()-(potrzebowalElementow));// bo byl przesuniety current na element z nastepnej serii
        longerTape->getStream().seekg((-doCofniecia)*sizeof(Cone),std::ios::cur);
    }



};


#endif //POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
