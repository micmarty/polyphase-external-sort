//
// Created by miczi on 28.10.16.
//

#ifndef POLYPHASE_EXTERNAL_SORT_CONE_H
#define POLYPHASE_EXTERNAL_SORT_CONE_H
#define M_PI 3.14159265358979323846

//MODIFY FLAG BELOW!!!!!!!!!!
#define DEBUG 0

class Cone {
    float radius;
    float height;

public:
    float getVolume() const{
#if DEBUG == 0
        return M_PI * radius * radius * height / 3.0;
#elif DEBUG == 1
        return radius;
#endif
    }

    Cone() {
        this->radius = 0;
        this->height = 0;
    }
    Cone(float radius, float height) {
        this->radius = radius;
        this->height = height;
    }
#if DEBUG == 0
//  ORYGINALNE
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
#else
    //  UPROSZCZONEEEEEEEEEEEEEEEE
    bool operator> (const Cone &right)const
    {
        return (radius > right.radius );
    }
    bool operator< (const Cone &right)const
    {
        return (radius  < right.radius );
    }
    bool operator>= (const Cone &right) const
    {
        return (radius  >= right.radius );
    }
    bool operator== (const Cone &right) const
    {
        return (radius == right.radius);
    }
    bool operator<= (const Cone &right)const
    {
        return (radius <= right.radius);
    }
#endif






};


#endif //POLYPHASE_EXTERNAL_SORT_CONE_H
