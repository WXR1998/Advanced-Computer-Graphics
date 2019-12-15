#ifndef LIGHT_H
#define LIGHT_H

#include "vecmath.h"

class Light {
private:
    Vector3f pos, color;
public:
    Vector3f getPosition() const{
        return pos;
    }
    Vector3f getColor() const{
        return color;
    }

    Light(const Light &a){
        pos = a.getPosition();
        color = a.getColor();
    }
    Light(Vector3f pos = Vector3f::ZERO, Vector3f color = Vector3f::ZERO):
        pos(pos), color(color) {}
};

#endif