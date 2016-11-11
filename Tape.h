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
    const std::string path;
    const std::string name;
    std::vector<Cone> buffer;
    std::fstream stream;

    int bufferSize;
    int readsFromTheDisk;
    int writesToTheDisk;
public:
    const int originalBufferSize;   //copy of buffer size that never changes!
    /* use fstream as writing stream */
    Tape(const std::string& path_,const std::string& name_,int bufferSize_) :
            path(path_),
            name(name_),
            stream(path_,std::ios::out | std::ios::binary),//open for writing
            buffer(),
            originalBufferSize(bufferSize_){

        bufferSize = originalBufferSize;
        buffer.reserve(originalBufferSize);

        readsFromTheDisk = 0;
        writesToTheDisk = 0;
    }

    /* use fstream as reading stream */
    Tape(const std::string& path_,const std::string& name_,int bufferSize_, bool readMode) :
            path(path_),
            name(name_),
            stream(path_,std::ios::in | std::ios::binary), //open for reading
            buffer(),
            originalBufferSize(bufferSize_){

        bufferSize = originalBufferSize;
        buffer.resize(originalBufferSize);

        readsFromTheDisk = 0;
        writesToTheDisk = 0;
    }

    Tape(Tape&);
    void operator= (Tape&);

    ~Tape(){
        stream.close();
    };

    //getters
    std::vector<Cone>& getBuffer(){
        return buffer;
    }
    std::fstream& getStream(){
        return stream;
    }
    std::string getName() const {
        return name;
    }
    std::string getPath() const {
        return path;
    }
    int getBufferSize() const {
        return bufferSize;
    }
    int getReadsFromTheDisk() const {
        return readsFromTheDisk;
    }
    int getWritesToTheDisk() const {
        return writesToTheDisk;
    }

    void display_buffer_content() {
        for(Cone c: buffer){
            std::cout<< c.getVolume() <<"\t";
        }
        std::cout<<std::endl;
    }

    void display_vector_content(std::vector<Cone>& vector) {
        for(Cone c: vector){
            std::cout<< c.getVolume() <<"\t";
        }
    }

    void flush_buffer_to_tape() {
        std::cout<<"~  czyszczenie resztek w buforze "<<name<<", zrzucam na tasme: ";
        display_buffer_content();
        persist_buffer();
        buffer.clear();
    }

    void insert_element_into_tape_buffer(Cone cone) {
        //  TODO simplify statement
        if(buffer.size() < bufferSize)       // push to the buffer
            buffer.push_back(cone);
        else if(buffer.size() == bufferSize){// persist buffer onto tape, then push cone into empty buffer
            std::cout<<"~  bufor "<<name<<" sie wypelnil, zrzucam na tasme: ";
            display_buffer_content();
            persist_buffer();
            buffer.clear();
            buffer.push_back(cone);
        }
        std::cout << "-> wpisano " << cone.getVolume() << " do bufora " << name << std::endl;
    }


    void persist_buffer(){
        stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(Cone));
        writesToTheDisk++;
    }

    //  put given vector onto the tape(called from ConeGenerator)
    void persist_vector(std::vector<Cone>& v){
        stream.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(Cone));
    }

    //  display one by one :(
    void display_tape(){
        int elementsCounter = 0;

        std::vector<Cone> tmpBuffer(1);
        std::cout<<"zawartość taśmy " + name << std::endl;
        while (stream.read(reinterpret_cast<char *>(tmpBuffer.data()), sizeof(Cone) * 1)){
            std::cout << tmpBuffer.at(0).getVolume() << "\t";
            elementsCounter++;
        }
        std::cout<<std::endl<<"posiada "<<elementsCounter<<" elementow"<<std::endl;
    }

    //only use if you've end up working with current stream and want switch to different mode
    std::streampos display_tape_but_save_position() {
        std::streampos savedPosition = stream.tellg();

        stream.clear();
        stream.open(path,std::ios::in|std::ios::binary);

        //  the reading and displaying part
        size_t bytesToRead = file_size();
        std::vector<Cone> tmpBuffer(bytesToRead/sizeof(Cone));
        std::cout<<"zawartość taśmy " + name << std::endl;
        stream.read(reinterpret_cast<char *>(tmpBuffer.data()), bytesToRead);
        display_vector_content(tmpBuffer);
        std::cout<<std::endl;

        //  turn writing mode
        stream.clear();
        //stream.close();
        stream.open(path,std::ios::out|std::ios::binary);
        stream.seekg(savedPosition);

        return savedPosition;
    }

    Cone last_from_buffer() {
        if(!buffer.empty())
            return buffer.back();
        else
            return Cone(INT_MIN, INT_MIN);
    }


/*  UTILITIES   */
    size_t file_size() {
        long begin = stream.tellg();    //  measure byte we are at
        stream.seekg (0, std::ios::end);//  go to the end
        long end = stream.tellg();      //  measure byte we are at
        stream.clear();                 //  clear bad and fail flag
        stream.seekg(0, std::ios::beg); //  reset position to the beginning

        return end - begin;             //  return file size in bytes
    }

    std::streamsize file_size_and_save_position() {
        std::streampos currentPosition = stream.tellg();
        stream.seekg( 0, std::ios::end );

        std::streamsize bytesLeft = stream.tellg() - currentPosition;

        stream.seekg(currentPosition);
        return bytesLeft;
    }

    std::pair<int, int> reads_and_writes()
    {
        return std::make_pair(readsFromTheDisk, writesToTheDisk);
    }


    void setBufferSize(int newSize) {
        bufferSize = newSize;
    }

    void incrementReadCounter() {
        readsFromTheDisk++;
    }

    void resizeBuffer(int newSize) {
        setBufferSize(newSize);
        buffer.resize(newSize);
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_TAPE_H
