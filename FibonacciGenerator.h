//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_FIBONACCIGENERATOR_H
#define POLYPHASE_EXTERNAL_SORT_FIBONACCIGENERATOR_H

#include <math.h>


class FibonacciGenerator {
    unsigned int fib1;
    unsigned int fib2;
    unsigned int newFib;
    bool secondOne;// 1 >1< 2 3 ...
public:
    FibonacciGenerator(){
        fib1 = 1;
        fib2 = 1;
        newFib = 1;
        secondOne = false;
    }

    unsigned int limit(){
        return newFib;
    }

    /*  generate next in sequence, shift fib1 and fib2 to new values    */
    unsigned int next(){
        // quick fix for two 1 at the beginning. I know it's ugly
        if(!secondOne && fib1 == 1 && fib2 == 1) {
            secondOne = true;
            return 1;
        }

        newFib = fib1 + fib2;
        fib1 = fib2;
        fib2 = newFib;
        return newFib;
    }

    static bool perfect_sqaure_number(int n) {
        if (n < 0)
            return false;
        else{
            if(sqrt(n)==floor(sqrt(n)))
                return true;
            else
                return false;
        }
    }

    static bool is_fib(int n){
        int tmp = 5*n*n;
        //binet's formula
        if(perfect_sqaure_number(tmp-4) || perfect_sqaure_number(tmp + 4))
            return true;
        else
            return false;
    }

    int nextFibFrom(const int &i) {
        do{
            next();
        }while(newFib <= i);
        return newFib;
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_FIBONACCIGENERATOR_H
