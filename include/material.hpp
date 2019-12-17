#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "bmp.hpp"
#include <iostream>

enum SurfaceProp {DIFFUSE, SPECULAR, TRANSPARENT};

class Material {
public:

    Material(const Vector3f &diffuseColor, 
        const Vector3f &specularColor = Vector3f::ZERO, 
        const Vector3f &shineColor = Vector3f::ZERO,
        bool isShine = false,
        SurfaceProp prop = DIFFUSE,
        double shineness = 0, 
        Bmp *texture = nullptr, 
        double iota = 1.5) :
            diffuseColor(diffuseColor),
            specularColor(specularColor), 
            shininess(shineness), 
            texture(texture), 
            prop(prop),
            iota(iota),
            isShine(isShine),
            shineColor(shineColor)
        {
    }

    ~Material() = default;

    Vector3f getDiffuseColor() const {
        return diffuseColor;
    }
    Vector3f getSpecularColor() const {
        return specularColor;
    }
    double getIota() const{
        return iota;
    }
    bool isShining() const{
        return isShine;
    }
    Vector3f getShineColor() const{
        return shineColor;
    }
    SurfaceProp getProperty() const{
        return prop;
    }
    Bmp* getTexture() const{
        return texture;
    }
    double getShineness() const{
        return shininess;
    }


    double min(double a, double b){ return a<b?a:b; }
    double max(double a, double b){ return a>b?a:b; }
    double clamp(double a){
        return min(max(a, 0), 1);
    }

protected:
    Vector3f diffuseColor;      // 漫反射颜色  
    Vector3f specularColor;     // 高光颜色
    Vector3f shineColor;        // 自发光颜色
    bool isShine;               // 是否自发光
    double shininess;            // 光泽度
    double iota;                 // 折射率
    SurfaceProp prop;           // 漫反射/全反射/透明
    Bmp *texture;
};


#endif // MATERIAL_H
