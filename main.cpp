#include <iostream>

#include "Distributor.h"
#include "ConeGenerator.h"
#include "MergingHandler.h"

#define LOAD_DATA_FROM_KEYBOARD_INPUT -1
#define LOAD_FROM_FILE 0
#define GENERATE_DATA 1


Tape* tape;
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

void load_from_keyboard(ConeGenerator* generator) {
    std::cout<<"Now you need, to give me records separated by spaces\nIn order to end the sequence give me -1 in input\n";

    //TODO load radius and height! so two arrays!
    std::vector<float> radiusVector;
    float radius;
    while ((std::cin >> radius) && radius != -1)
        radiusVector.push_back(radius);

    generator->import_from_vector(radiusVector, radiusVector, radiusVector.size()); //TODO replace second argument with heights
}

void load_from_file(ConeGenerator* generator) {
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
}

void populate_input_tape(std::string path) {
    ConeGenerator* generator = new ConeGenerator(path.c_str());
    int wayToPopulateInputTame = GENERATE_DATA;//choosen_data_source();

    if(wayToPopulateInputTame == LOAD_DATA_FROM_KEYBOARD_INPUT){
        load_from_keyboard(generator);
    }else if(wayToPopulateInputTame == LOAD_FROM_FILE){
        load_from_file(generator);
    }else if(wayToPopulateInputTame == GENERATE_DATA){
        std::cout<<"Now you need, to specify how many of records would you want to generate: ";
        int recordsToGenerate;
        //std::cin >> recordsToGenerate;
        generator->generate(13);
    }
    delete generator;
}

void display_generated_input_tape(std::string path) {
    Tape* inputTape = new Tape(path.c_str(),"INPUT", 1, true);
    inputTape->display_tape();
    delete inputTape;
}

std::pair<int,int> run_distribution_and_show_reads_and_writes(int bufferSize,std::string pathToINPUT, std::string pathToA, std::string pathToB) {
    std::cout<<"\n\n\n--- DISTRIBUTION PHASE ----\n";

    Distributor* distributor = new Distributor(bufferSize,pathToINPUT.c_str(),pathToA.c_str(),pathToB.c_str());
    std::pair<int,int> seriesAfterDistribution = distributor->distribute();
    delete(distributor);

    return seriesAfterDistribution;
}

void display_more_statistics_after_distribution(std::string pathA,std::string pathB) {
    tape = new Tape(pathA.c_str(),"A", 1,true);

    std::cout<<"\n\ntaśma A zawiera " << tape->file_size()<< " bajtów"<<std::endl;
    tape->display_tape();

    tape = new Tape(pathB.c_str(),"B", 1,true);

    std::cout<<"\n\ntaśma B zawiera " << tape->file_size()<< " bajtów"<<std::endl;
    tape->display_tape();

    delete tape;
}

void run_merging(std::string pathC,std::string pathA,std::string pathB, std::pair<int,int> series) {
    std::cout<<"\n\n\n--- SORTING AND MERGING PHASE ----\n";

    MergingHandler* mergingHandler = new MergingHandler(3,pathC,pathA,pathB);
    mergingHandler->run_merging_process(series.first,series.second);
    delete mergingHandler;
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
    std::pair<int,int> seriesAfterDistribution = run_distribution_and_show_reads_and_writes(bufferSize,tape_INPUT_path,tape_A_path,tape_B_path);

    //  DISPLAY A and B AFTER DISTRIBUTION
    display_more_statistics_after_distribution(tape_A_path, tape_B_path);

    //  SORTING
    run_merging(tape_C_path,tape_A_path, tape_B_path, seriesAfterDistribution);





    return 0;
}