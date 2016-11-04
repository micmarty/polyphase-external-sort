#include <iostream>

#include "Distributor.h"
#include "ConeGenerator.h"
#include "MergingHandler.h"


int main() {
    std::string projectPath = "/home/miczi/ClionProjects/polyphase-external-sort-edu/";
    std::string tape_INPUT_path = projectPath + "INPUT";
    std::string tape_A_path = projectPath + "A";
    std::string tape_B_path = projectPath + "B";
    std::string tape_C_path = projectPath + "C";

    //  GENERATE
    ConeGenerator* generator = new ConeGenerator(tape_INPUT_path.c_str());
    generator->generate(13);
    delete generator;

    //  DISPLAY GENERATED
    Tape* inputTape = new Tape(tape_INPUT_path.c_str(),"INPUT", 1, true);
    inputTape->display_tape();
    delete inputTape;


















    std::cout<<std::endl<<std::endl<<"--- DISTRIBUTION PHASE ----"<<std::endl;

    //  DISTRIBUTION -> specify bufferSize if you want to
    unsigned int bufferSize = 3;
    Distributor* distributor = new Distributor(bufferSize,tape_INPUT_path.c_str(),tape_A_path.c_str(),tape_B_path.c_str());
    distributor->distribute();
    delete(distributor);


    //  DISPLAY A and B AFTER DISTRIBUTION
    Tape* tape = new Tape(tape_A_path.c_str(),"A", 1,true);
    std::cout<<std::endl<<std::endl;
    std::cout<<"taśma A zawiera " << tape->file_size()<< " bajtów"<<std::endl;
    tape->display_tape();
    tape = new Tape(tape_B_path.c_str(),"B", 1,true);
    std::cout<<std::endl<<std::endl;
    std::cout<<"taśma B zawiera " << tape->file_size()<< " bajtów"<<std::endl;
    tape->display_tape();
    delete tape;


    //  SORTING
    std::cout<<std::endl<<std::endl<<"--- SORTING AND MERGING PHASE ----"<<std::endl;

    //  sorting and merging phase
    MergingHandler* mergingHandler = new MergingHandler(3,tape_C_path,tape_A_path,tape_B_path);
    mergingHandler->merge();
    delete mergingHandler;


    tape = new Tape(tape_C_path.c_str(),"C", 1,true);
    std::cout<<std::endl<<std::endl;
    tape->display_tape();




    return 0;
}