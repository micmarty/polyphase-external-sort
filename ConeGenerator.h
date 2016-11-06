//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H
#define POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H


#include <vector>
#include <iostream>
#include <stdlib.h>
#include "Cone.h"
#include "Tape.h"

// SEEMS TO BE DONE

class ConeGenerator {
    int bufferLimit;
    Tape* tapeToFill;
public:
    ConeGenerator(const std::string & path){
        tapeToFill = new Tape(path, "INPUT", 1);
    }

    ConeGenerator(ConeGenerator&);
    void operator= (ConeGenerator&);

    ~ConeGenerator(){
        delete tapeToFill;
    }

    void generate(int amount){
        //srand(time(NULL));
        std::vector<Cone> cones;
        //float tab[] = {10,8,1,2,2,7,9,2,5,8,9,8,8};
        float tab[] = {4,	8,	4,	7,	10,	3,	1,	4,	1,	3,	2,	8,	3,	3,	8,	10,	3,	10,	4,	2,	10,	2,	5,	9,	6,	4,	2,	7,	3,	7,	6,	5,	7,	7,	4,	5,	3,	5,	5,	4,	8,	7,	9,	4,	5};
                //{5,	8	,6	,7	,2	,7	,8	,5	,3,	3	,10	,4	,7	};

        std::cout<< "vector randomly generated:"<<std::endl;
        for (int i = 0; i < amount; i++) {
            float radius = tab[i];//(std::rand() % 10 + 1);
            float height = (std::rand() % 10 + 1);

            cones.push_back(Cone(radius, height));  //TODO replace with height
            std::cout << cones[i].getVolume() << "\t";
        }
        tapeToFill->persist_vector(cones);
        std::cout<<std::endl;
    }

    void import_from_vector(std::vector<float>& radiuses, std::vector<float>& heights, int amount) {
        std::vector<Cone> cones;

        std::cout<< "vector imported from file or keyboard"<<std::endl;
        for(int i=0;i<amount;i++){
            cones.push_back(Cone(radiuses[i], 0));   //TODO replace 0 with heights[i]
            std::cout << cones[i].getVolume() << "\t";
        }
        tapeToFill->persist_vector(cones);
        std::cout<<std::endl;
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H
