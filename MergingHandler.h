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


class MergingHandler {
    Tape destinationTape;
    Tape firstTape;
    Tape secondTape;
    Tape* currentTape;

    int indexFirst;
    int indexSecond;
    int* currentIndex;

public:
    MergingHandler(int bufferSize_,
                   const std::string& destinationPath,
                   const std::string& firstPath,
                   const std::string& secondPath):
            destinationTape(destinationPath,"DESTINATION",bufferSize_),
            firstTape(firstPath, "FIRST", bufferSize_,true),
            secondTape(secondPath, "SECOND", bufferSize_,true)
    {
        indexFirst = 0;
        indexSecond = 0;
        currentTape = &firstTape;
        currentIndex = &indexFirst;
    }

    ~MergingHandler()
    {

    }

    Tape* other_tape(){
        if(currentTape == &firstTape){
            return &secondTape;
        }else if(currentTape == &secondTape){
            return &firstTape;
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


    void run_merging_process(int seriesOnA, int seriesOnB){
        //  obie są fibonacim
        //  destination juz jest C i na zapis
        if(FibonacciGenerator::is_fib(std::max(seriesOnA,seriesOnB))){
            Tape* shorterTape = (std::min(seriesOnA,seriesOnB))?&firstTape:&secondTape;
            shorterTape = merge(destinationTape);
            shorterTape.open_for_writing();
            destinationTape.open_for_reading();
        }else{
            //  trzeba obliczyc ilosc dummy serii
        }
    }

    void merge(){
        using namespace std;
        //  read one chunk from these 2 input tapes
        firstTape.getStream().read(reinterpret_cast<char *>(firstTape.getBuffer().data()), sizeof(Cone) * firstTape.getBuffer().size());
        secondTape.getStream().read(reinterpret_cast<char *>(secondTape.getBuffer().data()), sizeof(Cone) * secondTape.getBuffer().size());

        firstTape.readsFromTheDisk++;
        secondTape.readsFromTheDisk++;

        //DEBUG
        firstTape.display_buffer_content();
        secondTape.display_buffer_content();


        currentIndex = &indexFirst;
        currentTape = &firstTape;

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
                    destinationTape.insert_element_into_tape_buffer(elementOnCurrent);//wpisz

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
                destinationTape.flush_buffer_to_tape();
                std::cout<<"natychmiast wychodze " << std::endl;
                break;
            }
        }


    }




};


#endif //POLYPHASE_EXTERNAL_SORT_MERGINGHANDLER_H
