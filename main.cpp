#include <iostream>

#include "Distributor.h"
#include "ConeGenerator.h"



int main() {
    std::string projectPath = "/home/miczi/ClionProjects/polyphase-external-sort-edu/";
    std::string tape_INPUT_path = projectPath + "INPUT";
    std::string tape_A_path = projectPath + "A";
    std::string tape_B_path = projectPath + "B";

    //  0. phase
    //ConeGenerator* generator = new ConeGenerator(tape_INPUT_path.c_str());
    //generator->generate(31);
    //delete generator;

    //  0.1 test written records
    Tape* inputTape = new Tape(tape_INPUT_path.c_str(),"INPUT", 1, true);
    inputTape->display_tape();
    delete inputTape;

    // 1. phase
    unsigned int bufferSize = 3;
    Distributor* distributor = new Distributor(bufferSize,tape_INPUT_path.c_str(),tape_A_path.c_str(),tape_B_path.c_str());
    distributor->distribute();
    delete(distributor);
    return 0;
}