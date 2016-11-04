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
    std::string path;
    std::string name;
    std::vector<Cone> buffer;
    std::fstream stream;

public:
    unsigned int bufferSize;

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

    void flush_buffer_to_tape() {
        std::cout<<"~  czyszczenie resztek w buforze "<<name<<", zrzucam na tasme: ";
        display_buffer_content();

        persist_buffer();

        buffer.clear();
    }

    void insert_element_into_tape_buffer(Cone cone) {
        //  TODO simplify statement
        if(buffer.size() < bufferSize) //   push to buffer
            buffer.push_back(cone);
        else if(buffer.size() == bufferSize){// persist buffer
            std::cout<<"~  bufor "<<name<<" sie wypelnil, zrzucam na tasme: ";
            display_buffer_content();
            std::cout<<std::endl;

            persist_buffer();

            buffer.clear();
            buffer.push_back(cone);
        }
        std::cout << "-> wpisano " << cone.getVolume() << " do bufora " << name << std::endl;
    }




    void persist_buffer(){
        stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(Cone));
    }

    //  put given vector onto the tape
    void persist_vector(std::vector<Cone>& v){
        stream.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(Cone));
    }

    //  display one by one :(
    void display_tape(){
        int elementsCounter = 0;

        std::cout<<"zawartość taśmy " + name << std::endl;
        while (stream.read(reinterpret_cast<char *>(buffer.data()), sizeof(Cone) * 1)){
            std::cout << buffer.at(0).getVolume() << "\t";
            elementsCounter++;
        }
        std::cout<<std::endl<<"posiada "<<elementsCounter<<" elementow"<<std::endl;
    }

    Cone last_from_buffer() {
        if(!buffer.empty())
            return buffer.back();
        else
            return Cone(INT_MIN, INT_MIN);
    }


    size_t file_size() {
        long begin = stream.tellg();    //  measure byte we are at
        stream.seekg (0, std::ios::end);//  go to the end
        long end = stream.tellg();      //  measure byte we are at
        stream.clear();                 //  clear bad and fail flag
        stream.seekg(0, std::ios::beg); //  reset position to the beginning

        return end - begin;             //  return file size in bytes
    }

    size_t file_size_and_save_position() {
        std::streampos currentPosition = stream.tellg();
        stream.seekg( 0, std::ios::end );

        std::streamsize bytesLeft = stream.tellg() - currentPosition;

        stream.seekg(currentPosition);
        return bytesLeft;
    }

};


#endif //POLYPHASE_EXTERNAL_SORT_TAPE_H
