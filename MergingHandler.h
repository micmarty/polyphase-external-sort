//
// Created by miczi on 30.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
#define POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H


#include <vector>
#include <algorithm>
#include <iostream>
#include "Cone.h"
#include "Tape.h"


#define JEST_W_BUFORZE 0
#define TRZEBA_ZALADOWAC_BUFOR 1
#define KONIEC_TASMY -1

#define A 1
#define B 2
#define C 3

class MergingHandler {
    Tape cTape;
    Tape aTape;
    Tape bTape;

    Tape* shorterTape;
    Tape* longerTape;
    Tape* destinationTape;

    Tape* currentTape;

    int indexFirst;
    int indexSecond;
    int* currentIndex;
    int bufferSize;

public:
    MergingHandler(int bufferSize_,
                   const std::string& cPath,
                   const std::string& aPath,
                   const std::string& bPath):
            cTape(cPath,"C",bufferSize_),
            aTape(aPath, "A", bufferSize_,true),
            bTape(bPath, "B", bufferSize_,true)
    {
        indexFirst = 0;
        indexSecond = 0;
        currentTape = &aTape;
        currentIndex = &indexFirst;
        bufferSize = bufferSize_;
    }

    ~MergingHandler()
    {

    }

    Tape* other_tape(){
        if(currentTape == shorterTape){
            return longerTape;
        }else if(currentTape == longerTape){
            return shorterTape;
        }
    }

    int* other_index(){
        if(currentIndex == &indexFirst)
            return &indexSecond;
        else if(currentIndex == &indexSecond)
            return &indexFirst;
    }

    void zaladuj_kolejny_bufor() {
        size_t bajtowDoKonca = currentTape->file_size_and_save_position();
        int rekordowDoKonca = bajtowDoKonca/sizeof(Cone);
        *currentIndex = -1;  //resetujemy tutaj licznik na poczatek bufora

        //  zaladuj pelny bufor
        if(rekordowDoKonca >= currentTape->bufferSize){
            currentTape->getStream().read(reinterpret_cast<char *>(currentTape->getBuffer().data()), sizeof(Cone) * currentTape->getBuffer().size());
            currentTape->readsFromTheDisk++;
        }
        //  zaladuj pomniejszony bufor
        else if(rekordowDoKonca < currentTape->bufferSize){
            //  zaktualizuj rozmiary bufora
            currentTape->bufferSize = rekordowDoKonca;
            currentTape->getBuffer().resize(rekordowDoKonca);

            currentTape->getStream().read(reinterpret_cast<char *>(currentTape->getBuffer().data()), sizeof(Cone) * currentTape->getBuffer().size());
            currentTape->readsFromTheDisk++;
        }

    }

    bool istnieje_bufor_do_zaladowania() {
        size_t bajtowDoKonca = currentTape->file_size_and_save_position();
        int rekordowDoKonca = bajtowDoKonca/sizeof(Cone);

        //  nie ma wiecej buforow na tasmie, bo jestesmy na koncu!
        if(rekordowDoKonca == 0){
            return false;
        }else{
            return true;
        }
    }

    bool istnieje_bufor_do_zaladowania_na_other() {
        size_t bajtowDoKonca = other_tape()->file_size_and_save_position();
        int rekordowDoKonca = bajtowDoKonca/sizeof(Cone);

        //  nie ma wiecej buforow na tasmie, bo jestesmy na koncu!
        if(rekordowDoKonca == 0){
            return false;
        }else{
            return true;
        }
    }

    int status_sasiada() {
        int neigh_index = (*currentIndex) + 1;

        //  sasiad lezy jeszcze w obrebie obecnego bufora
        if(neigh_index < currentTape->getBuffer().size()){                  std::cout<<"Nie przekroczymy tasmy. Obecny: "<<currentTape->getBuffer().at(*currentIndex).getVolume()<<", sasiad: "<<currentTape->getBuffer().at(neigh_index).getVolume()<<std::endl;
            return JEST_W_BUFORZE;   //  nie przekroczy bo jest w zasiegu bufora
        }
        //  sasiad lezy poza buforem, trzeba zaladowac nowy bufor(pelny, lub pomniejszony - ostatni)
        else{
            if(istnieje_bufor_do_zaladowania()){
                return TRZEBA_ZALADOWAC_BUFOR;
            }else{                                                          std::cout<<"Przekroczymy tasme, bo nie ma dalej żadnej serii."<<std::endl;
                return KONIEC_TASMY;
            }
        }
    }

    int status_sasiada_na_other() {
        int neigh_index = (*other_index()) + 1;

        //  sasiad lezy jeszcze w obrebie obecnego bufora
        if(neigh_index < other_tape()->getBuffer().size()){                  std::cout<<"Nie przekroczymy tasmy. Obecny: "<<other_tape()->getBuffer().at(*other_index()).getVolume()<<", sasiad: "<<other_tape()->getBuffer().at(neigh_index).getVolume()<<std::endl;
            return JEST_W_BUFORZE;   //  nie przekroczy bo jest w zasiegu bufora
        }
            //  sasiad lezy poza buforem, trzeba zaladowac nowy bufor(pelny, lub pomniejszony - ostatni)
        else{
            if(istnieje_bufor_do_zaladowania_na_other()){
                return TRZEBA_ZALADOWAC_BUFOR;
            }else{                                                          std::cout<<"Przekroczymy tasme, bo nie ma dalej żadnej serii."<<std::endl;
                return KONIEC_TASMY;
            }
        }
    }

    bool sasiad_nalezy_do_serii(Cone current) {
        //  porownaj obecny z sasiadujacym
        if(current <= currentTape->getBuffer().at((*currentIndex) + 1)){
            std::cout<<"Sąsiad należy do serii."<<std::endl;
            return true;
        }
        std::cout<<"Sąsiad należy do nowej serii."<<std::endl;
        return false;
    }

    Cone sasiad_current() {
        (*currentIndex) += 1;   //przesuwamy indeks w prawo
        return currentTape->getBuffer().at((*currentIndex));
    }

    Cone zmien_tasme() {
        std::cout<<"Zmieniam taśmę."<<std::endl;
        currentTape = other_tape();
        currentIndex = other_index();
        return currentTape->getBuffer().at(*currentIndex);
    }

    void set_as_destination(int tape){
        if(tape == A){
            destinationTape = &aTape;
        }else if(tape == B){
            destinationTape = &bTape;
        }else if(tape == C){
            destinationTape = &cTape;
        }
    }

    void reopen_longer_for_reading(std::streampos pos) {
        longerTape->getStream().close();
        longerTape->getStream().open(longerTape->getPath(),std::ios::in|std::ios::binary);
        //longerTape->getStream().seekg(pos);
    }

    void reopen_destination_for_writing() {
        destinationTape->getStream().close();
        destinationTape->getStream().open(destinationTape->getPath(),std::ios::out|std::ios::binary);
    }

    void reopen_shorter_for_reading(std::streampos pos) {
        shorterTape->getStream().open(shorterTape->getPath(),std::ios::in|std::ios::binary);
        shorterTape->getStream().clear();
        //shorterTape->getStream().seekg(pos);
    }


    std::pair<std::streampos,std::streampos> display_tapes_after_merge() {
        std::cout<<"\n\nNastąpiło scalenie taśm :\n";

        //shorter
        std::streampos shorterStoppedHere = shorterTape->display_tape_but_save_position();
        shorterTape->getStream().open(shorterTape->getPath(),std::ios::in|std::ios::binary);
        shorterTape->getStream().seekg(shorterStoppedHere);

        //longer
        std::streampos longerStoppedHere = longerTape->display_tape_but_save_position();
        longerTape->getStream().seekg(longerStoppedHere);

        //destination
        std::cout<<"na taśmę destination:\n";
        destinationTape->display_tape_but_save_position();
        destinationTape->getStream().seekg(0, std::ios::beg);
        std::cout<<std::endl;

        //  all of them are now in writing mode
        return std::pair<std::streampos,std::streampos>(shorterStoppedHere,longerStoppedHere);
    }

    void swap_roles(Tape* nextShorter) {
        Tape* savedDestination = destinationTape;
        destinationTape = shorterTape;
        shorterTape = nextShorter;
        longerTape = savedDestination;
    }



    void run_merging_process(int seriesOnA, int seriesOnB){
        //  obie są fibonacim
        destinationTape = &cTape;
        shorterTape = (std::min(seriesOnA,seriesOnB))?&aTape:&bTape;
        longerTape = (shorterTape==&aTape)?&bTape:&aTape;// choose the one that is left

        if(FibonacciGenerator::is_fib(std::max(seriesOnA,seriesOnB))){
            for(int stage=0;stage<2;stage++){
                Tape* nextShorter = merge();

                //std::pair<std::streampos,std::streampos> shorterLongerEndPair = display_tapes_after_merge();
                swap_roles(nextShorter);
                reopen_destination_for_writing();
                reopen_longer_for_reading(0);
                reopen_shorter_for_reading(0);//shortStoppedHere
            }
        }else{
            //  trzeba obliczyc ilosc dummy serii
        }
    }

    void reset_buffer_sizes() {
        shorterTape->getBuffer().resize(bufferSize);
        longerTape->getBuffer().resize(bufferSize);
        destinationTape->getBuffer().clear();
    }



    Tape* merge(){  //  zwraca tasme na ktorej przerwano scalanie
        using namespace std;
        //  read one chunk from these 2 input tapes
        //shorterTape->getStream().read(reinterpret_cast<char *>(shorterTape->getBuffer().data()), sizeof(Cone) * shorterTape->getBuffer().size());
        //longerTape->getStream().read(reinterpret_cast<char *>(longerTape->getBuffer().data()), sizeof(Cone) * longerTape->getBuffer().size());

        currentIndex = &indexFirst;
        currentTape = shorterTape;

        reset_buffer_sizes();

        if(istnieje_bufor_do_zaladowania()){
            zaladuj_kolejny_bufor();shorterTape->readsFromTheDisk++;*currentIndex = 0;
        }
        zmien_tasme();
        if(istnieje_bufor_do_zaladowania()){
            zaladuj_kolejny_bufor();longerTape->readsFromTheDisk++;*currentIndex = 0;
        }
        zmien_tasme();

        shorterTape->display_buffer_content();
        longerTape->display_buffer_content();


        //  wez po jednym rekordzie z kazdego bufora
        Cone elementOnCurrent = currentTape->getBuffer().at(*currentIndex);
        Cone elementOnOther = other_tape()->getBuffer().at(*other_index());

        bool napotkanoKoniecJednejZTasm = false;
        for(int scalanieSerii=1;scalanieSerii<=13;scalanieSerii++){
            int napotkanoKoniecSerii = 0;

            while(napotkanoKoniecSerii != 2){
                cout<<elementOnCurrent.getVolume()<<"<="<<elementOnOther.getVolume()<<endl;
                if(elementOnCurrent <= elementOnOther || napotkanoKoniecSerii==1) {
                    //  wstaw mniejszy z dwoch
                    destinationTape->insert_element_into_tape_buffer(elementOnCurrent);//wpisz

                    //  obczaj gdzie jest sasiad, cyz w ogole jest ?!?
                    int akcjaPrzedUstawieniemSasiada = status_sasiada();    //  JEST_W_BUFORZE || TRZEBA_ZALADOWAC_BUFOR || KONIEC_TASMY

                    //  jesli sasiad gdzies jest
                    if (akcjaPrzedUstawieniemSasiada == JEST_W_BUFORZE ||
                        akcjaPrzedUstawieniemSasiada == TRZEBA_ZALADOWAC_BUFOR) {

                        // ale jest na nastepnym buforze
                        if (akcjaPrzedUstawieniemSasiada == TRZEBA_ZALADOWAC_BUFOR)
                            //  to pobierz bufor
                            zaladuj_kolejny_bufor();    // i wyzeruj index

                        //  jesli jest <=, to on staje sie currenetm teraz
                        if (sasiad_nalezy_do_serii(elementOnCurrent)) {
                            elementOnCurrent = sasiad_current();
                        } else {
                            //  jesli nie, to znaczy ze jest poczatkiem nowej serii, wracamy wiec na druga tasme
                            //skoro opuszczamy te tasme to trzeba przywrocic index na nową serię
                            *currentIndex += 1;
                            napotkanoKoniecSerii++;
                            elementOnOther = elementOnCurrent;
                            elementOnCurrent = zmien_tasme();
                        }
                        // jesli sasiad obecnego elementu juz nie istnieje, bo skonczyla sie tasma
                    } else if (akcjaPrzedUstawieniemSasiada == KONIEC_TASMY) {
                        napotkanoKoniecJednejZTasm = true;
                        elementOnOther = elementOnCurrent;
                        elementOnCurrent = zmien_tasme();
                        napotkanoKoniecSerii++;

                        *other_index() -= 1;//chcial wskazywac na nastepna juz serie, a tego nie chcemy
                    }

                }
                else{
                    elementOnOther = elementOnCurrent;
                    elementOnCurrent = zmien_tasme();
                }


            }

            if(!napotkanoKoniecJednejZTasm){
                //zakonczylismy poprzednia, serie i przesuwamy currenty na elementy z nowej serii
                if(status_sasiada() != KONIEC_TASMY){
                    elementOnCurrent = currentTape->getBuffer().at(*currentIndex);
                    cout<<endl<<endl<<endl<<"przygotowanie do " <<scalanieSerii + 1<< "serii, elementOnCurrent = " << elementOnCurrent.getVolume()<<" ,index = " <<*currentIndex<<endl;
                }
                if(status_sasiada_na_other() != KONIEC_TASMY){
                    elementOnOther = other_tape()->getBuffer().at(*other_index());
                    cout<<"przygotowanie do " <<scalanieSerii + 1<< "serii, elementOnOther = " << elementOnOther.getVolume()<<" ,index = " <<*other_index()<<endl;
                }
            }else{
                destinationTape->flush_buffer_to_tape();
                std::cout<<"natychmiast wychodze " << std::endl;

                cofnijWskaznikiTasm();

                //break;
                return currentTape;//bo po napotkaniu konca, zmienilismy tasme!
            }
        }
    }

    void cofnijWskaznikiTasm() {

        int maxIndexBuforaCurrent = currentTape->getBuffer().size() - 1;
        int bajtowDoCofnieciaNaTasmieCurrent = sizeof(Cone) * (maxIndexBuforaCurrent - (*currentIndex));
        other_tape()->getStream().seekg(-bajtowDoCofnieciaNaTasmieCurrent,std::ios::cur);


        int maxIndexBuforaOther = other_tape()->getBuffer().size() - 1;
        int bajtowDoCofnieciaNaTasmieOther = sizeof(Cone) * (maxIndexBuforaOther - (*other_index()));
        currentTape->getStream().seekg(-bajtowDoCofnieciaNaTasmieOther,std::ios::cur);


    }




};


#endif //POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
