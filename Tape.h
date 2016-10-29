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
    std::string getName(){
        return name;
    }
    std::string getPath(){
        return path;
    }
    void display_buffer_content() {
        for(Cone c: buffer){
            std::cout<< c.getVolume() <<"\t";
        }
        std::cout<<std::endl;
    }

    void insert_element_into_tape_buffer(Cone cone) {
        if(buffer.size() < bufferSize) //   push to buffer
            buffer.push_back(cone);
        else if(buffer.size() == bufferSize){// persist buffer
            std::cout<<"~  bufor "<<name<<" sie wypelnil, zrzucam na tasme: ";
            display_buffer_content();
            std::cout<<std::endl;
            buffer.clear();

            buffer.push_back(cone);
        }
        std::cout << "-> wpisano " << cone.getVolume() << " do bufora " << name << std::endl;
    }


    Tape(const std::string& path_,const std::string& name_,unsigned int bufferSize_) : stream(path_,std::ios::out | std::ios::binary),buffer(){
        //open for writing
        name = name_;
        path = path_;
        bufferSize= bufferSize_;
        buffer.reserve(bufferSize_);
    }

    Tape(const std::string& path_,const std::string& name_,unsigned int bufferSize_, bool readMode) : stream(path_,std::ios::in | std::ios::binary),buffer(){
        //open for reading
        name = name_;
        path = path_;
        bufferSize = bufferSize_;
        buffer.resize(bufferSize_);
    }

    Tape(Tape&);
    void operator= (Tape&);

    ~Tape(){stream.close();};

    //  put given vector onto the tape
    void persist_vector(std::vector<Cone>& v){
        stream.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(Cone));
    }

    //  display one by one :(
    void display_tape(){
        std::cout<<"zawartość taśmy " + name << std::endl;
        while (stream.read(reinterpret_cast<char *>(buffer.data()), sizeof(Cone) * 1)){
            std::cout << buffer.at(0).getVolume() << "\t";
        }
        std::cout<<std::endl;
    }

    Cone last_from_buffer() {
        if(!buffer.empty())
            return buffer.back();
        else
            return Cone(INT_MIN, INT_MIN);
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_TAPE_H
