//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_CONE_H
#define POLYPHASE_EXTERNAL_SORT_CONE_H


#include <lcms.h>

class Cone {
    float radius;
    float height;

public:
    float getVolume() const{
        return M_PI * radius * radius * height / 3.0;
        //return radius; //DEBUG
    }

    Cone() {
        this->radius = 0;
        this->height = 0;
    }
    Cone(float radius, float height) {
        this->radius = radius;
        this->height = height;
    }

    //  operators overloading for comparisons
    bool operator> (const Cone &right)const
    {
        return (getVolume() > right.getVolume() );
    }
    bool operator< (const Cone &right)const
    {
        return (getVolume()  < right.getVolume() );
    }
    bool operator>= (const Cone &right) const
    {
        return (getVolume()  >= right.getVolume() );
    }

    bool operator== (const Cone &right) const
    {
        return (getVolume() == right.getVolume());
    }



    bool operator<= (const Cone &right)const
    {
        return (getVolume() <= right.getVolume());
    }
};


#endif //POLYPHASE_EXTERNAL_SORT_CONE_H
