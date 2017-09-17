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



#define IS_IN_THE_BUFFER 0
#define NEED_TO_GET_NEW_BUFFER 1
#define END_OF_TAPE -1

#define GET_LAST -1
#define END_OF_DUMMIES -2

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

    Tape* currentTape;  //points to shorter or longerTape pointer

    int indexShorter;
    int indexLonger;
    int* currentIndex;//points to one of the above indexes

    bool freshOnShorter;
    bool freshOnLonger;
    bool *freshBufferOnCurrent;//points to one of the above booleans

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

    /* returns boolean flag on the opposite tape    */
    bool *opposite_fresh() {
        if(freshBufferOnCurrent == &freshOnShorter)
            return &freshOnLonger;
        else if(freshBufferOnCurrent == &freshOnLonger)
            return &freshOnShorter;
    }


    void try_to_load_next_chunk_into_current_tape() {
        //  COUNT HOW MANY RECORDS LEFT ON CURRENT TAPE
        std::streampos bytesLeft = currentTape->file_size_and_save_position();
        int recordsLeft = bytesLeft/ sizeof(Cone);

        //  RESET VALUES -> enable flag indicating fresh buffer
        *currentIndex = 0;  //reset position counter to the beginning of new buffer
        *freshBufferOnCurrent = true;

        //  DETERMINE IF WE CAN AFFORD USING FULL BUFFER, OR JUST DIMINISHED ONE(LAST ON TAPE)
            //  load full buffer(with original, maximum size given in constructor)
        if(recordsLeft >= currentTape->getBufferSize())
            currentTape->getStream().read(reinterpret_cast<char *>(currentTape->getBuffer().data()), sizeof(Cone) * currentTape->getBuffer().size());
        else{
            //  load diminished buffer(the last in the tape)
            //  update fields
            currentTape->resizeBuffer(recordsLeft);
            currentTape->getStream().read(reinterpret_cast<char *>(currentTape->getBuffer().data()), sizeof(Cone) * recordsLeft);
        }

        //  INCREASE DISK READ OPERATION
        currentTape->incrementReadCounter();

    }

    /*  just checks if there are some records to load in    */
    bool is_data_to_fill_the_buffer() {
        //  COUNT HOW MANY RECORDS LEFT
        size_t bytesLeft = currentTape->file_size_and_save_position();
        int recordsLeft = bytesLeft/sizeof(Cone);

        //  DETERMINE IF WE'VE FOUND END_OF_TAPE STATUS
        if(recordsLeft == 0){   //  there's no more data for buffer to load
            return false;
        }else{
            return true;        //  there's something we can load into the buffer
        }
    }

    /*  just finds out, what do we need to get neighbour, but just check, work on local vars    */
    int neighbour_status() {
        int neigh_index = (*currentIndex) + 1;

            //  NEIGHBOUR IS REACHABLE IN CURRENT BUFFER
        if(neigh_index < currentTape->getBuffer().size()){
            if(neigh_index == 0)
                std::cout<<"Nie przekroczymy tasmy. Obecny: poprzedni bufor, sasiad: "<<currentTape->getBuffer().at(neigh_index).getVolume()<<std::endl;
            else
                std::cout<<"Nie przekroczymy tasmy. Obecny: "<<currentTape->getBuffer().at(*currentIndex).getVolume()<<", sasiad: "<<currentTape->getBuffer().at(neigh_index).getVolume()<<std::endl;
            return IS_IN_THE_BUFFER;   //  nie przekroczy bo jest w zasiegu bufora
        }
        else{
            //NEIGHBOUR IS OUTSIDE CURRENT BUFFER, WE NEED TO LOAD FULL OR DIMINISHED BUFFER(LAST IN THE TAPE)
            if(is_data_to_fill_the_buffer()){
                return NEED_TO_GET_NEW_BUFFER;
            }else{
                std::cout<<"Przekroczymy tasme, bo nie ma dalej żadnej serii."<<std::endl;
                return END_OF_TAPE;
            }
        }
    }

    /*  check if after current element is the beginning of the new series   */
    bool neighbour_comes_from_the_current_series(Cone current) {
        //  TAKE CARE OF GOOD INDEXES
        int neigh_index;
        if(*freshBufferOnCurrent)   //  current is last in the buffer, so its neighbour is at 0 in next buffer
            neigh_index = 0;
        else                        //  typical situation, we go 1 index to the right
            neigh_index = *currentIndex + 1;

        //  MAKE COMPARISON TO DETERMINE IF NEIGHBOUR IS FROM CURRENT SERIES
        //  compare current with next
        if(current <= currentTape->getBuffer().at(neigh_index)){
            std::cout<<"Sąsiad należy do serii."<<std::endl;
            return true;
        } else{
            std::cout<<"Sąsiad należy do nowej serii."<<std::endl;
            return false;
        }
    }

    /*  move currentIndex to its neighbour index
     *  return its value
     * */
    Cone neighbour_of_current() {
        //  TAKE CARE OF GOOD INDEXES
        int neigh_index;
        if(*freshBufferOnCurrent){
            neigh_index = 0;
            *freshBufferOnCurrent = false;
        }
        else
            neigh_index = *currentIndex + 1;

        //  UPDATE THE INDEX
        (*currentIndex) = neigh_index;

        //  RETURN THE NEIGHBOUR
        return currentTape->getBuffer().at(neigh_index);
    }


    /*  switch between two tapes, choose the opposite one
     * take care of using proper indexes and flags
     * */
    Cone change_tape() {
        std::cout<<"Zmieniam taśmę."<<std::endl;
        //  SWITCH ALL POINTERS TO THE OPPOSITE
        currentTape = opposite_tape();
        currentIndex = opposite_index();
        freshBufferOnCurrent = opposite_fresh();

        //  RETURN ELEMENT WE'VE END UP WITH PREVIOUSLY
        return currentTape->getBuffer().at(*currentIndex);
    }

    /*  switches from writing to reading
     * (because current longer was destination tape a moment ago!!!
     * */
    void reopen_longer_for_reading() {
        longerTape->getStream().close();
        longerTape->getStream().open(longerTape->getPath(),std::ios::in|std::ios::binary);
    }

    /*  open from position we've end up with
     * (because current shorter was longer tape a moment ago
     * we want to read series that are still there!!!
     * */
    void reopen_shorter_for_reading() {
        shorterTape->getStream().open(shorterTape->getPath(),std::ios::in|std::ios::binary);
        shorterTape->getStream().clear();
    }

    /*  switches from reading to writing
     * (because current destination was shorter tape a moment ago!!!
     * */
    void reopen_destination_for_writing() {
        destinationTape->getStream().close();
        destinationTape->getStream().open(destinationTape->getPath(),std::ios::out|std::ios::binary);
    }

    /*  switch the roles of each tape after merging, before next merging phase*/
    void swap_roles(Tape* nextShorter) {
        //  SHORTER BECOMES DESTINATION
        //  LONGER BECOMES SHORTER
        //  DESTINATION BECOMES LONGER
        Tape* savedDestination = destinationTape;
        destinationTape = shorterTape;
        shorterTape = nextShorter;
        longerTape = savedDestination;
    }

    /*  MAIN FUNCTION CALLED FROM MAIN.CPP
     * parameters: it needs to know which tape is longer/shorter    */
    void run_merging_process(int seriesOnA, int seriesOnB){
        //  DECLARE FLAGS AND INITIAL STATES
        bool endOfBothTapesFound = false;
        int dummiesLeft = 0;

        //  we assume A and B are input, C is output
        destinationTape = &cTape;
        //  choose shorter and longer based on arguments given: seriesOnA, seriesOnB
        shorterTape = (std::min(seriesOnA,seriesOnB) == seriesOnA)?&aTape:&bTape;
        longerTape = (shorterTape==&aTape)?&bTape:&aTape;


            //  IF BOTH ARE NON-EQUAL FIBONACCI NUMBERS(PERFECT SITUATION -> no dummy series)
        if(FibonacciGenerator::is_fib(seriesOnA) && FibonacciGenerator::is_fib(seriesOnB) && seriesOnA != seriesOnB){
            //  RUN MERGING PROCESS AS LONG AS THERE ARE SERIES TO MERGE
            while(true){
                //  WHOLE MECHANISM IS HERE
                Tape* nextShorter = merge(dummiesLeft, endOfBothTapesFound);

                //  EXIT THE WHILE LOOP IF NECESSARY
                if(endOfBothTapesFound)
                    break;

                //  PREPARE FOR NEXT ITERATION
                swap_roles(nextShorter);
                reopen_destination_for_writing();
                reopen_longer_for_reading();
                reopen_shorter_for_reading();
            }
        }else{
            //  IF ONE TAPE HAS FIBONACCI NUMBER OR TAPES HAVE THE SAME SERIES NUMBER

            //  if we need to generate artigicial series, called -> dummy series
            std::cout<<"TAŚMY NIE SĄ PERFEKCYJNYMI LICZBAMI FIBONACCIEGO, OBLICZAM DUMMY\n";
            FibonacciGenerator generator = FibonacciGenerator();

                //  if tapes has the same series number
                //  or
                //  A is fib -> B
            if(seriesOnA==seriesOnB || FibonacciGenerator::is_fib(seriesOnA)){
                //  add dummies to bTape
                dummiesLeft = generator.nextFibFrom(seriesOnA) - seriesOnB;
                shorterTape = &aTape;
                longerTape = &bTape;
            }else{
                //  if tapes has different series number
                //  and
                //  A is not fib -> so B is

                //  add dummies to aTape
                dummiesLeft = generator.nextFibFrom(seriesOnB) - seriesOnA;
                shorterTape = &bTape;
                longerTape = &aTape;
            }

            //  :( COPIED FRAGMENT THAT IS IDENTICAL WITH FIBONACCI PERFECT SITUATION
            while(true) {
                Tape *nextShorter = merge(dummiesLeft, endOfBothTapesFound);

                if (endOfBothTapesFound)
                    break;

                swap_roles(nextShorter);
                reopen_destination_for_writing();
                reopen_longer_for_reading();
                reopen_shorter_for_reading();
            }

        }

        //  WE'RE DONE WITH MERGING
        cout<<"SCALANIE ZAKOŃCZONE. TAŚMA POWINNA BYĆ POSORTOWANA"<<endl;
    }

    /*  renew all sizes, and delete trash   */
    void reset_buffers() {
        //  clear() -> changes size to 0 and removes all the data
        shorterTape->getBuffer().clear();
        longerTape->getBuffer().clear();
        destinationTape->getBuffer().clear();

        //  restore value that may have been changed somewhere
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

    /*  assign values that are connected somewhat with shorter tape */
    void start_with_shorter_tape() {
        currentIndex = &indexShorter;
        currentTape = shorterTape;
        freshBufferOnCurrent = &freshOnShorter;
    }

    /*  handle dummies, take control over the counter(how many dummies left), set flags */
    void reactToDummies(int& dummiesLeft, bool& forceRewritingSeries, int& endOfSeriesFoundCounter) {

            //  we still need some series to be merged with dummies
        if(dummiesLeft > 0){
            forceRewritingSeries = true;    //  flag that allows us to avoid comparison and picking up lower value
            endOfSeriesFoundCounter = 0;    //  reset the value during rewriting process, because there may be multiple series to rewrite
            dummiesLeft--;                  //  must-have, decrease counter

        }else if(dummiesLeft == 0){
            //  we've met last dummy

            cout<<"Serie dummy już się skończyły, rusza normalne scalanie z obu taśm\n";
            dummiesLeft = GET_LAST;         //  set a flag that forces us to move to the neighbour element, before switching to normal merging mode
            forceRewritingSeries = false;   //  we don't need that flag anymore

        }else if(dummiesLeft == GET_LAST){
            //  last situation

            dummiesLeft = END_OF_DUMMIES;   //  avoid all dummy-connected if statements inside merge function
            forceRewritingSeries = false;   //  we don't need that flag anymore, just to be sure
        }
    }

    /*  returns longer tape, just in case to continue further merging   */
    /*  it's basically the whole mechanism of merging, here  */
    Tape* merge(int& dummiesLeft, bool& endOfBothTapesFound){
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
            int endOfSeriesFoundCounter = 0;    //  values between 0 .. 2 , 2 means we can start merging new pair of series
            bool forceRewritingSeries = false;  //  if there are dummies, we need to avoid comparing values
                                                //  that we are rewriting with those, from the opposite tape

            //  modify dummies counter, eventually display message if we're done with them
            reactToDummies(dummiesLeft,forceRewritingSeries,endOfSeriesFoundCounter);

            //  if on both tapes we've found the beginning of the new series, exit
            while(endOfSeriesFoundCounter != 2){
                //  DISPLAY COMPARED VALUES
                cout<<elementOnCurrent.getVolume()<<" <= "<<elementOnOther.getVolume()<<endl;

                //  typical comparison between two tapes
                //  OR
                //  one of the tape found end of the series, so the opposite tape needs to rewrite all elements until it finds the second series end
                //  OR
                //  we are rewriting(merging with empty dummies) series from shorter tape(the one that has NO DUMMY ON IT)
                if(elementOnCurrent <= elementOnOther || endOfSeriesFoundCounter == 1 || forceRewritingSeries){
                    destinationTape->insert_element_into_tape_buffer(elementOnCurrent);

                    //  IS_IN_THE_BUFFER || NEED_TO_GET_NEW_BUFFER || END_OF_TAPE
                    int actionBeforeChangingToNeighbour = neighbour_status();

                    //  NO END_OF_TAPE FOUND, SO CONTINUE THE ALGORITHM
                    if(actionBeforeChangingToNeighbour == IS_IN_THE_BUFFER || actionBeforeChangingToNeighbour == NEED_TO_GET_NEW_BUFFER){

                        //  MAYBE WE WOULD NEED TO LOAD NEXT BUFFER IN ORDER TO HAVE ACCESS TO CURRENT's ELEMENT NEIGHBOUR
                        if(actionBeforeChangingToNeighbour == NEED_TO_GET_NEW_BUFFER){
                            //  load and reset currentIndex to 0, freshBufferLoaded to true
                            try_to_load_next_chunk_into_current_tape(); //   at this point we KNOW that some buffer exist to load
                            cout<<"ZALADOWALEM KOLEJNY BUFOR!:"<<endl;
                            currentTape->display_buffer_content();
                        }

                            //  if currentElement <= its neighbour, then use him as currentElement
                        if(neighbour_comes_from_the_current_series(elementOnCurrent)){
                            elementOnCurrent = neighbour_of_current();
                        }else{
                            //  otherwise, we've found the beginning of the new series
                            //  so we need to leave current tape and go back to the other one
                            endOfSeriesFoundCounter++;

                            //  REFRESH DUMMIES COUNTER
                            reactToDummies(dummiesLeft,forceRewritingSeries,endOfSeriesFoundCounter);

                                //  typical situation(if we're out of dummies): swap elements/change the tape
                            if(dummiesLeft == END_OF_DUMMIES){
                                elementOnOther = elementOnCurrent;
                                elementOnCurrent = change_tape();
                            }else{
                                //  if some dummies are left, then always switch to neighbour, and rewrite
                                elementOnCurrent = neighbour_of_current();
                                endOfSeriesFoundCounter = 0;    //  during dummies merging, we don't care about changing tapes
                            }

                        }
                    }else if(actionBeforeChangingToNeighbour == END_OF_TAPE){
                        //  END_OF_TAPE FOUND

                        //  we've found end of BOTH tapes, so we've merged 1 series and 1 series, then exit.
                        //  it exits WHOLE ALGORITHM(we are modifying a reference to this variable and it's declared in run_merging_process
                        if(endOfOneTapeFound){
                            endOfBothTapesFound = true;
                        }

                        //  we've found tape end for the first time
                        endOfOneTapeFound = true;
                        //  swap the tapes
                        elementOnOther = elementOnCurrent;
                        elementOnCurrent = change_tape();   // extract element we've end up previously
                        endOfSeriesFoundCounter++;
                    }
                }else{
                    //  if elemOnCurrent is not <= elemOnOther
                    //  we need to swap the tapes and run the loop again
                    elementOnOther = elementOnCurrent;
                    elementOnCurrent = change_tape();
                }

            }

            seriesMerged++; //  useful information, but unnecessary

                //  AT THIS POINT WE CAN EXIT THIS PHASE, WE'VE MERGED TWO TAPES INTO DESTINATION TAPE
            if(endOfOneTapeFound){
                destinationTape->flush_buffer_to_tape();
                std::cout<<"FAZA SORTOWANIA SKOŃCZONA" << std::endl;
                rewindUnusedBuffer();  //  read method description

                //  longer will be shorter(and we need that pointer to swap in run_merging_process)
                return longerTape;//    EXIT ENDLESS LOOP
            }else{
                //  NO TAPE END FOUND, SO WE NEED TO START FROM THE NEXT SERIES


                //  TAKE CARE OF CURRENT TAPE
                if(!(*freshBufferOnCurrent))    //  no excessive buffer had to be loaded
                    *currentIndex += 1;             //  move to the element from the new series
                elementOnCurrent = currentTape->getBuffer().at(*currentIndex);  //  update elementOnCurrent
                cout<<"\n\n\nprzygotowanie do " <<seriesMerged + 1<< "serii, elementOnCurrent = " << elementOnCurrent.getVolume()<<" ,index = " <<*currentIndex<<endl;

                //  TAKE CARE OF THE OPPOSITE TAPE
                if(!*opposite_fresh())
                    *opposite_index() += 1; //move to the element from the new series

                elementOnOther = opposite_tape()->getBuffer().at(*opposite_index());//  update elementOnCurrent
                cout<<"przygotowanie do " <<seriesMerged + 1<< "serii, elementOnOther = " << elementOnOther.getVolume()<<" ,index = " <<*opposite_index()<<endl;
            }
        }
    }

    /*  very important operation before next merging phase
     * longer tape will become shorter, because there are some series left on that tape
     * it is possible that algorithm has used loaded unnecessary buffer(excessive)
     * and because of that it consumed too much elements
     * we need here to calculate how many of them was unnecessary, and rewind the tape by x records
     * */
    void rewindUnusedBuffer() {
        //  (5 6| 3 2 1)
        //          ^ - position we've end up with
        //  index = 3, but we need to give back 4 elements
        int elementReallyUsed = indexLonger + 1;

        if(freshOnLonger)   //  a whole buffer loaded, but no element used
            elementReallyUsed = 0;

        //  CALCULATE HOW MUCH TO REWIND AND REWIND
        int elementsToRewind = (longerTape->getBuffer().size()-(elementReallyUsed));
        longerTape->getStream().seekg((-elementsToRewind)*sizeof(Cone),std::ios::cur);
    }



};


#endif //POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
