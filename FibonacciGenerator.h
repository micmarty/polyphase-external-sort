//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_FIBONACCIGENERATOR_H
#define POLYPHASE_EXTERNAL_SORT_FIBONACCIGENERATOR_H


class FibonacciGenerator {
    unsigned int fib1;
    unsigned int fib2;
    unsigned int newFib;
public:
    FibonacciGenerator(){
        fib1 = 1;
        fib2 = 1;
        newFib = 1;
    }

    unsigned int limit(){
        return newFib;
    }

    /*  generate next in sequence, shift fib1 and fib2 to new values    */
    unsigned int next(){
        newFib = fib1 + fib2;
        fib1 = fib2;
        fib2 = newFib;
        return newFib;
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_FIBONACCIGENERATOR_H
