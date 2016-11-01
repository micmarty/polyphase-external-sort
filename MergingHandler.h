//
// Created by miczi on 30.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
#define POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H


#include <vector>
#include <algorithm>
#include <iostream>
#include "Cone.h"
#include "Tape.h"

class MergingHandler {
    Tape destinationTape;
    Tape firstTape;
    Tape secondTape;

    int indexFirst;
    int indexSecond;

public:
    MergingHandler(unsigned int bufferSize_,
                   const std::string& destinationPath,
                   const std::string& firstPath,
                   const std::string& secondPath):
            destinationTape(destinationPath,"DESTINATION",bufferSize_),
            firstTape(firstPath, "FIRST", bufferSize_,true),
            secondTape(secondPath, "SECOND", bufferSize_,true)
    {
        indexFirst = 1;
        indexSecond = 1;
    }

    ~MergingHandler()
    {

    }

    std::vector<unsigned int> pushSeriesToDestinationBuffer(){
        unsigned int breakIndexOnA = 0, breakIndexOnB = 0;
        std::vector<Tape*>tapes{&firstTape,&secondTape};

        for(Tape* currentTape: tapes){
            Cone last = currentTape->getBuffer().front();
            destinationTape.getBuffer().push_back(last);

            for(unsigned int tapeBufferIndex=1; tapeBufferIndex< currentTape->getBuffer().size(); tapeBufferIndex++){
                Cone currentElement = currentTape->getBuffer().at(tapeBufferIndex);
                if(last <= currentElement){
                    destinationTape.getBuffer().push_back(currentElement);
                }else
                {
                    currentTape==&firstTape ? breakIndexOnA=tapeBufferIndex:breakIndexOnB=tapeBufferIndex;
                    break;
                }
            }
        }
        return std::vector<unsigned int>{breakIndexOnA,breakIndexOnB};
    }

    Tape* other_tape(Tape* currentTape){
        if(currentTape == &firstTape){
            return &secondTape;
        }else if(currentTape == &secondTape){
            return &firstTape;
        }
    }

    int* other_index(int* currentIndex){
        if(currentIndex == &indexFirst)
            return &indexSecond;
        else if(currentIndex == &indexSecond)
            return &indexFirst;
    }

    void merge(){
        //  read one chunk from these 2 input tapes
        firstTape.getStream().read(reinterpret_cast<char *>(firstTape.getBuffer().data()), sizeof(Cone) * firstTape.getBuffer().size());
        secondTape.getStream().read(reinterpret_cast<char *>(secondTape.getBuffer().data()), sizeof(Cone) * secondTape.getBuffer().size());

        //DEBUG
        firstTape.display_buffer_content();
        secondTape.display_buffer_content();


        std::vector<Cone> sorted;

        //  wybor pierwszej tasmy
        Tape* currentTape = &firstTape;
        int* currentIndex = &indexFirst;

        //  wez po jednym rekordzie z kazdego bufora
        Cone elementOnCurrent = currentTape->getBuffer().at(*currentIndex);
        Cone elementOnOther = other_tape(currentTape)->getBuffer().at(*other_index(currentIndex));

        jeszcze_raz:
        if(elementOnCurrent <= elementOnOther){
            do{
                sorted.push_back(elementOnCurrent);     //  wsadz ten element
                std::cout<<"puushuje "<< elementOnCurrent.getVolume() <<"\n";
                (*currentIndex)++;                      //sproboj przesunac sie do nastepnego

                //jesli wyszlismy za rozmiar bufora po poprzediej inkrementacji
                if(*currentIndex == currentTape->getBuffer().size()){
                    //wczytaj kolejny chunk do bufora
                    currentTape->getStream().read(reinterpret_cast<char *>(currentTape->getBuffer().data()), sizeof(Cone) * currentTape->getBuffer().size());
                    *currentIndex = 0;      //ustaw licznik na 0
                }

                //sprawdzmy czy element ktory zaraz podstawimy nie bedzie juz czasem z nowej serii
                if (elementOnCurrent <= currentTape->getBuffer().at(*currentIndex))
                    elementOnCurrent = currentTape->getBuffer().at(*currentIndex);
                else{
                    //przeskocz na druga tasme

                }

                if(elementOnCurrent > elementOnOther){
                    zmien_tasme = true;

                    currentTape = other_tape(currentTape);
                    currentIndex = other_index(currentIndex);
                    elementOnCurrent = currentTape->getBuffer().at(*currentIndex);
                    elementOnOther = other_tape(currentTape)->getBuffer().at(*other_index(currentIndex));
                    goto jeszcze_raz;
                }



            }while(zmien_tasme != true);
        }

//        if(currentOnSecond<=currentOnSecond){
//            do{
//                sorted.push_back(currentOnSecond);
//                std::cout<<"puushuje "<< currentOnSecond.getVolume() <<"\n";
//                lastOnSecIndex++;
//                currentOnSecond = secondTape.getBuffer().at(lastOnSecIndex);
//            }while(currentOnSecond <= currentOnSecond);
//        }
    }

};


#endif //POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
