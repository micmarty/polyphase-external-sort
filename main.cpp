#include <iostream>

#include "Distributor.h"
#include "ConeGenerator.h"
#include "MergingHandler.h"

#define LOAD_DATA_FROM_KEYBOARD_INPUT -1
#define LOAD_FROM_FILE 0
#define GENERATE_DATA 1

int choosen_data_source() {
    std::cout<<"Good morning, sir\n";
    std::cout<<"Choose your input data source: \n\t<k> for keyboard \n\t<f> for file \n\t<g> for auto-generated data\n";

    std::string userDecision;
    std::cin >> userDecision;

    if(userDecision == "k")
        return LOAD_DATA_FROM_KEYBOARD_INPUT;
    else if(userDecision == "f")
        return LOAD_FROM_FILE;
    else if(userDecision == "g"){
        return GENERATE_DATA;
    }
}

void populate_input_tape(std::string path) {
    ConeGenerator* generator = new ConeGenerator(path.c_str());
    int wayToPopulateInputTame = choosen_data_source();

    if(wayToPopulateInputTame == LOAD_DATA_FROM_KEYBOARD_INPUT){
        std::cout<<"Now you need, to give me records separated by spaces\nIn order to end the sequence give me -1 in input\n";

        //TODO load radius and height! so two arrays!
        std::vector<float> radiusVector;
        float radius;
        while ((std::cin >> radius) && radius != -1)
            radiusVector.push_back(radius);

        generator->import_from_vector(radiusVector, radiusVector, radiusVector.size()); //TODO replace second argument with heights
    }else if(wayToPopulateInputTame == LOAD_FROM_FILE){
        std::ifstream inputData("/home/miczi/ClionProjects/polyphase-external-sort/input.txt");
        if (inputData) {
            std::vector<float> radiusVector;
            float radius;

            // read the elements in the file into a vector
            while ( inputData >> radius ) {
                radiusVector.push_back(radius);
            }
            generator->import_from_vector(radiusVector, radiusVector, radiusVector.size()); //TODO replace second argument with heights
        }
    }else if(wayToPopulateInputTame == GENERATE_DATA){
        std::cout<<"Now you need, to specify how many of records would you want to generate: ";
        int recordsToGenerate;
        std::cin >> recordsToGenerate;
        generator->generate(recordsToGenerate);
    }

    delete generator;
}

void display_generated_input_tape(std::string path) {
    Tape* inputTape = new Tape(path.c_str(),"INPUT", 1, true);
    inputTape->display_tape();
    delete inputTape;
}

void run_distribution(int bufferSize,std::string pathToINPUT, std::string pathToA, std::string pathToB) {
    std::cout<<"\n\n\n--- DISTRIBUTION PHASE ----\n";

    Distributor* distributor = new Distributor(bufferSize,pathToINPUT.c_str(),pathToA.c_str(),pathToB.c_str());
    distributor->distribute();
    delete(distributor);
}

int main() {
    std::string projectPath = "/home/miczi/ClionProjects/polyphase-external-sort-edu/";
    std::string tape_INPUT_path = projectPath + "INPUT";
    std::string tape_A_path = projectPath + "A";
    std::string tape_B_path = projectPath + "B";
    std::string tape_C_path = projectPath + "C";



    populate_input_tape(tape_INPUT_path);
    display_generated_input_tape(tape_INPUT_path);

    int bufferSize = 3;
    run_distribution(bufferSize,tape_INPUT_path,tape_A_path,tape_B_path);




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