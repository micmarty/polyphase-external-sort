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
    int bufferSize;
    Tape* tapeToFill;
public:
    ConeGenerator(const std::string & path, int bufferSize_):
            bufferSize(bufferSize_){
        tapeToFill = new Tape(path, "INPUT", bufferSize_);
    }

    ConeGenerator(ConeGenerator&);
    void operator= (ConeGenerator&);

    ~ConeGenerator(){
        delete tapeToFill;
    }

    void generate(int amount){
        srand(time(NULL));

//        //  DEBUG
//        float tab[] = {4,	8,	4,	7,	10,
//                       3,	1,	4,	1,	3,
//                       2,	8,	3,	3,	8,
//                       10,	3,	10,	4,	2,
//                       10,	2,	5,	9,	6,
//                       4,	2,	7,	3,	7,
//                       6,	5,	7,	7,	4,
//                       5,	3,	5,	5,	4,
//                       8,	7,	9,	4,	5};

//        float tab[] = {
//                7,	2,	5,	8,	3,
//                9,	8,	9,	8,	9,
//                1,	4,	8,	6,	7,
//                9,	1,	3,	7,	5,
//                6,	4,	4,	3,	7,
//                6,	4,	10,	9,	8,
//                10,	7,	10,	3,	1,
//                8,	7,	6,	1,	3,
//                9,	6,	4,	7,	2,
//                2,	7,	3,	4,	6,
//                1,	8,	6,	10,	6,
//                10,	4,	10,	7,	6,
//                5,  4,  3,  2,  1
//        };

//        float tab[] = {
//                6,	4,	2,	4,	1,
//                3,	2,	2,	3,	6,
//                8,	2,	7,	4,	5,
//                8,	5,	2,	6,	6,
//                8,	8,	3,	10,	9,
//                8,	4,	6,	8,	10,
//                10,	1,	3,	5,	5,
//                3,	8,	3,	4,	4,
//                4,	2,	9,	7,	6,
//                10,	10,	8,	4,	9,
//                7,	4,	8,	10,	8,
//                8,	1,	10,	2,	8,
//                10,	3,	1,	6,	4,
//                4,	5,	3,	3,	1,
//                4,	2,	6,	5,	1,
//                9,	1,	7,	1,	1,
//                2,	9,	1,	5,	5,
//                10,	7,	9,	8,	5,
//                1,	5,	3,	9,	9,
//                3,	8,	3,	6,	9
//        };

//        float tab[] = { 7,	3,	7,	4,	2,
//                        3,	7,	1,	3,	5,
//                        2,	9,	10,	4,	8,
//                        10,	2,	10,	9,	6};
//
//        int tabSize = sizeof(tab)/sizeof(tab[0]);
//        amount = tabSize;
        //  END DEBUG

        std::cout<< "vector randomly generated:"<<std::endl;
        for (int i = 0; i < amount; i++) {
            float radius = (std::rand() % 10 + 1);
            float height = (std::rand() % 10 + 1);

            Cone cone = Cone(radius, height);
            std::cout << cone.getVolume() << "\t";
            tapeToFill->insert_element_into_tape_buffer(cone);
        }
        tapeToFill->flush_buffer_to_tape();
        std::cout<<std::endl;
    }

    void import_from_vector(std::vector<float>& radiuses, std::vector<float>& heights, int amount) {
        std::vector<Cone> cones;

        std::cout<< "vector imported from file or keyboard"<<std::endl;
        for(int i=0;i<amount;i++){
            cones.push_back(Cone(radiuses[i], 0));   //TODO replace 0 with heights[i]
            std::cout << cones[i].getVolume() << "\t";

            if(i % bufferSize == 0){
                tapeToFill->persist_vector(cones);
                cones.clear();
            }
        }
        tapeToFill->persist_vector(cones);
        std::cout<<std::endl;
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H
