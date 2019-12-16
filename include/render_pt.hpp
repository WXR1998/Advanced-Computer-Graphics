#ifndef RENDER_H
#define RENDER_H

#include <cstdlib>
#include <iostream>
#include <cstdio>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "bmp.hpp"
#include "bezier.hpp"
#include "ray.hpp"

#include <string>
#include <array>
#include <plane.hpp>
#include <cstdlib>

extern const double eps;
extern bool debug;
extern const int MAX_DEP;

/*
    深度限制的
    给定入射光线r，求其颜色
*/
Vector3f getColor(const Ray &r, int dep, Group *baseGroup){
    if (dep >= MAX_DEP || r.getDirection().isBlack())
        return Vector3f::ZERO;
    Hit hit;
    bool isIntersect = baseGroup->intersect(r, hit, eps * 50);  // 由于浮点误差，tmin必须为一个正数，防止光线穿模
    if (isIntersect){
        GeneratedRays gr = hit.getGeneratedRays();
        Material *mat = hit.getMaterial();

        if (mat->isShining())   // 是自发光物体
            return mat->getShineColor();

        Vector3f res = Vector3f::ZERO;
        Vector3f R = Vector3f::randomRay(hit.getIncidenceNormal());

        switch (mat->getProperty()){
            case DIFFUSE:
                R = Vector3f::randomRay(hit.getIncidenceNormal());
                assert(Vector3f::dot(R, hit.getIncidenceNormal()) >= 0);
                res = getColor(Ray(hit.getCollision(), R), 
                    dep+1, baseGroup) * hit.getMaterial()->getDiffuseColor();
                break;

            case SPECULAR:
                res = getColor(gr.reflect, dep+1, baseGroup) * 
                    hit.getMaterial()->getSpecularColor();
                break;

            case TRANSPARENT:
                res = gr.reflectRatio * getColor(gr.reflect, dep+1, baseGroup) * 
                    hit.getMaterial()->getSpecularColor() +
                    gr.refractRatio * getColor(gr.refract, dep+1, baseGroup);
                break;
        }

        if (hit.getTextureCoor() != nullptr && hit.getMaterial()->getTexture() != nullptr)
            res = res * hit.getMaterial()->getTexture()->
                GetSmoothColor(hit.getTextureCoor()->y(), hit.getTextureCoor()->x());
        return res;
    }
    else
        return Vector3f::ZERO;
}


#endif