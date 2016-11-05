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
    bool zuzyto;
public:
    FibonacciGenerator(){
        fib1 = 1;
        fib2 = 1;
        newFib = 1;
        zuzyto = false;
    }

    unsigned int limit(){
        return newFib;
    }

    /*  generate next in sequence, shift fib1 and fib2 to new values    */
    unsigned int next(){
        if(!zuzyto && fib1 == 1 && fib2 == 1)
        {
            zuzyto = true;
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
};


#endif //POLYPHASE_EXTERNAL_SORT_FIBONACCIGENERATOR_H
