//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H
#define POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H


#include <vector>
#include <iostream>
#include "Cone.h"
#include "Tape.h"

class ConeGenerator {
    int bufferLimit;
    Tape tapeToFill;
public:
    ConeGenerator(const std::string & path): tapeToFill(path, "INPUT"){
        bufferLimit = 31;
    }
    ~ConeGenerator(){

    }
    void generate(unsigned int amount){
        float tab[] = {10, 8, 2, 1, 2, 2, 7, 9, 8, 8, 5, 8, 9, 4, 7, 1, 6, 1, 6, 6, 8, 5, 2, 4, 10, 2, 3, 6, 1, 3, 1};
        std::vector<Cone> cones;

        std::cout<< "wektor wygenerowany:"<<std::endl;
        for (int i = 0; i < amount; i++) {
            float radius = tab[i];//(std::rand() % 10 + 1);
            //float height = (float) (std::rand() % 10 + 1);

            cones.push_back(Cone(radius, radius));
            std::cout << cones[i].getVolume() << "\t";

            if(cones.size() == bufferLimit){
                tapeToFill.persist_vector(cones);
                cones.clear();
            }

        }
        std::cout<<std::endl;
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H
