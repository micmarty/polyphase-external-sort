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

class ConeGenerator {
    int bufferLimit;
    Tape* tapeToFill;
public:
    ConeGenerator(const std::string & path){
        tapeToFill = new Tape(path, "INPUT", 1);
        bufferLimit = 0;
    }

    ConeGenerator(ConeGenerator&);
    void operator= (ConeGenerator&);

    ~ConeGenerator(){
        delete tapeToFill;
    }
    void generate(unsigned int amount){
        bufferLimit = amount;

        srand(time(NULL));
        //DEBUG
         //{963, 1184, 938, 1215, 620, 1030, 1163, 760, 270, 1631, 679, 906, 353, 563, 1333, 637, 1194, 1402, 482, 937, 341, 645, 1213, 1592, 1006, 510, 246, 433, 788, 1378, 465, 1294, 460, 418, 284, 628, 560, 1287, 1325, 1501, 521, 1076, 1165, 1292, 1629, 1194, 154, 881, 1683, 468, 415, 838, 1523, 1267, 162, 807, 274, 272, 1350, 533, 1320, 1093, 157, 493, 620, 704, 912, 403, 433, 954, 1393, 1323, 759, 1097, 1540, 1319, 1666, 1621, 1195, 988, 992, 897, 197, 1032, 1321, 1508, 1261, 1064, 470, 1445, 1153, 10, 1331, 1205, 1124, 448, 401, 1273, 730, 891, 948, 1807, 538, 1123, 1317, 874, 978, 228, 1426, 1288, 987, 325, 1201, 934, 1213, 457, 130, 1082, 335, 599, 4, 1249, 1433, 73, 258, 570, 1579, 200, 69, 768, 437, 1500, 228, 269, 1736, 375, 1608, 855, 710, 1447, 1111, 64, 191, 955, 1291, 727, 846, 1479, 1104, 370, 736, 970, 213, 1513, 1448, 1563, 1461, 829, 199, 257, 661, 739, 1378, 1602, 1389, 614, 640, 1485, 194, 853, 341, 593, 399, 706, 995, 787, 181, 352, 666, 57, 1230, 975, 733, 662, 803, 60, 772, 364, 1204, 199, 384, 720, 972, 173, 372, 13, 833, 615, 295, 382, 1002, 1386, 879, 287, 689, 322, 1540, 1107, 1030, 69, 1047, 1163, 936, 50, 283, 535, 166, 747, 1180, 210, 517, 939, 1010, 864, 701, 1655, 1633, 1394, 1028, 1463, 81, 1201, 1550, 917, 1185, 143, 444, 742, 352, 169, 150, 1316, 1063, 673, 624, 1165, 1117, 422, 1117, 920, 598, 630, 547, 1119, 1571, 1627, 626, 802, 637, 45, 394, 428, 399, 519, 107, 419, 197, 579, 1066, 1498, 855, 970, 679, 1765, 475, 1619, 512, 839, 1105, 1291, 13, 428, 261, 512, 52, 1732, 1038, 776, 1021, 328};/
        //{10, 8, 2, 1, 2, 2, 7, 9, 8, 8, 5, 8, 9};//, 4, 7, 1, 6, 1, 6, 6, 8, 5, 2, 4, 10, 2, 3, 6, 1, 3, 1};
        float tab[] = { 7	,1,	1	,2,	7	,3,	5	,9	,5	,9	,9	,6	,2};
        std::vector<Cone> cones;

        std::cout<< "wektor wygenerowany:"<<std::endl;
        for (int i = 0; i < amount; i++) {
            float radius = tab[i];//(std::rand() % 10 + 1);//DEBUG
            float height = (float) (std::rand() % 100 + 1);

            cones.push_back(Cone(radius, radius));//DEBUG
            std::cout << cones[i].getVolume() << "\t";

            if(cones.size() == bufferLimit){
                tapeToFill->persist_vector(cones);
                cones.clear();
            }

        }
        std::cout<<std::endl;
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_CONEGENERATOR_H
