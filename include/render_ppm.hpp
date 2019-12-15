#ifndef RENDER_PPM_H
#define RENDER_PPM_H

#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <string>
#include <array>
#include <vector>

#include "vecmath.h"
#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "bmp.hpp"
#include "bezier.hpp"
#include "ray.hpp"
#include "plane.hpp"
#include "kdtree.hpp"

extern const double eps;
extern const int MAX_DEP;

/*
    相机发出光线ray，其在图像buffer中的下标为index，把所有漫反射点放入sightpoint
    colorCoef:  当前走过的光路中，各种衰减的叠加
*/
void camera_pass(Ray ray, int dep, int index, std::vector<SPPMNode> &sightpoint, double radius, Group *group, Vector3f colorCoef = Vector3f(1, 1, 1)){
    if (dep > MAX_DEP) return;
    Hit hit;
    bool isIntersect = group->intersect(ray, hit, eps * 50);
    if (isIntersect){
        GeneratedRays gr = hit.GeneratedRays();
        Material *mat = hit.getMaterial();
        Vector3f textureCoef = Vector3f(1, 1, 1);

        if (hit.getTextureCoor() != nullptr && hit.getMaterial()->getTexture() != nullptr)
            textureCoef = hit.getMaterial()->getTexture()->
                GetSmoothColor(hit.getTextureCoor()->y(), hit.getTextureCoor()->x());
        switch (mat->getProperty()){
            case DIFFUSE:
                sightpoint.push_back(SPPMNode(hit.getCollision(), 
                    mat->getDiffuseColor() * colorCoef * textureCoef, 
                    hit.getIncidenceNormal(), radius, index));
                break;
            case SPECULAR:
                camera_pass(gr.reflect, dep+1, index, sightpoint, radius, group, 
                    colorCoef * mat->getSpecularColor() * textureCoef);
                break;
            case TRANSPARENT:
                camera_pass(gr.reflect, dep+1, index, sightpoint, radius, group, 
                    colorCoef * mat->getSpecularColor() * gr.reflectRatio * textureCoef);
                camera_pass(gr.refract, dep+1, index, sightpoint, radius, group, 
                    colorCoef * gr.refractRatio * textureCoef);
                break;
        }
    }
}

/*
    光源发出光子ray，其颜色为col，统计其对哪些像素的颜色有贡献
    colorCoef:  当前走过的光路中，各种衰减的叠加
*/
void light_pass(Ray ray, int dep, int color, PixelColor *c, KDTree &tree, Group *group, 
    Vector3f colorCoef = Vector3f(1, 1, 1)){
    if (dep > MAX_DEP) return;
    Hit hit;
    bool isIntersect = group->intersect(ray, hit, eps * 50);
    if (isIntersect){

    }
}

#endif