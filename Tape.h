//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_TAPE_H
#define POLYPHASE_EXTERNAL_SORT_TAPE_H

#include <string>
#include <vector>
#include <fstream>
#include "Cone.h"

class Tape {

private:
    unsigned int bufferSize;
    std::string path;
    std::string name;
    std::vector<Cone> buffer;
    std::fstream stream;


public:
    std::vector<Cone>& getBuffer(){
        return buffer;
    }
    std::fstream& getStream(){
        return stream;
    }

    void insert_element_into_tape_buffer(){

    }


    Tape(const std::string& path_,const std::string& name_,unsigned int bufferSize) : stream(path_,std::ios::out | std::ios::binary),buffer(3){
        //open for writing
        name = name_;
        path = path_;
    }

    Tape(const std::string& path_,const std::string& name_,unsigned int bufferSize, bool readMode) : stream(path_,std::ios::in | std::ios::binary),buffer(3){
        //open for reading
        name = name_;
        path = path_;
    }

    //  put given vector onto the tape
    void persist_vector(std::vector<Cone>& v){
        stream.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(Cone));
    }

    //  display one by one :(
    void display_tape(){
        stream.close();
        stream.open(path.c_str(), std::ios::in | std::ios::binary);

        std::cout<<"zawartość taśmy " + name << std::endl;
        while (stream.read(reinterpret_cast<char *>(buffer.data()), sizeof(Cone) * 1)){
            std::cout << buffer.at(0).getVolume() << "\t";
        }
        std::cout<<std::endl;

        stream.close();
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_TAPE_H
