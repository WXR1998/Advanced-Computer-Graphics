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

Vector3f specularShineness(Vector3f specCol, Vector3f textCol, double shineness){
    return specCol * shineness + textCol * (1 - shineness);
}

/*
    相机发出光线ray，其在图像buffer中的下标为index，把所有漫反射点放入sightpoint
    colorCoef:  当前走过的光路中，各种衰减的叠加
*/
void camera_pass(Ray ray, int dep, int index, std::vector<SPPMNode> &sightpoint, double radius, Group *group, Vector3f colorCoef = Vector3f(1, 1, 1), double prob = 1.0){
    if (dep > MAX_DEP || ray.getDirection().isBlack() || prob < eps) return;
    if (colorCoef.isBlack()) return;
    Hit hit;
    bool isIntersect = group->intersect(ray, hit, eps * 50);
    if (isIntersect){
        GeneratedRays gr = hit.getGeneratedRays();
        Material *mat = hit.getMaterial();
        Vector3f textureCoef = Vector3f(1, 1, 1);   // 在照相机发出光线的过程中，采样点已经计入该表面颜色！

        if (hit.getTextureCoor() != nullptr && hit.getMaterial()->getTexture() != nullptr)
            textureCoef = hit.getMaterial()->getTexture()->
                GetSmoothColor(hit.getTextureCoor()->y(), hit.getTextureCoor()->x());
        double P;
        switch (mat->getProperty()){
            case DIFFUSE:
                sightpoint.push_back(SPPMNode(hit.getCollision(), 
                    mat->getDiffuseColor() * colorCoef * textureCoef, 
                    hit.getIncidenceNormal(), radius, index, prob));
                break;
            case SPECULAR:
                camera_pass(gr.reflect, dep+1, index, sightpoint, radius, group, 
                    colorCoef * specularShineness(mat->getSpecularColor(), textureCoef, 
                    mat->getShineness()), prob);
                break;
            case TRANSPARENT:
                P = 0.25 + 0.5 * gr.reflectRatio;
                if (dep > 2){
                    if (erand48(X) < P)
                        camera_pass(gr.reflect, dep+1, index, sightpoint, radius, group, 
                            colorCoef * specularShineness(mat->getSpecularColor(), textureCoef, mat->getShineness()), 
                            prob * gr.reflectRatio / P);
                    else
                        camera_pass(gr.refract, dep+1, index, sightpoint, radius, group, 
                            colorCoef * mat->getDiffuseColor() * textureCoef, prob * gr.refractRatio / (1 - P));
                }else{
                    camera_pass(gr.reflect, dep+1, index, sightpoint, radius, group, 
                        colorCoef * specularShineness(mat->getSpecularColor(), textureCoef, mat->getShineness()), prob * gr.reflectRatio);
                    camera_pass(gr.refract, dep+1, index, sightpoint, radius, group, 
                        colorCoef * mat->getDiffuseColor() * textureCoef, prob * gr.refractRatio);
                }
                break;
        }
    }
}

/*
    光源发出光子ray，其颜色为col，统计其对哪些像素的颜色有贡献
*/
void light_pass(Ray ray, int dep, Vector3f color, PixelColor *c, KDTree &tree, Group *group, double prob = 1.0){
    if (dep > MAX_DEP || ray.getDirection().isBlack() || prob < eps) return;
    Hit hit;
    bool isIntersect = group->intersect(ray, hit, eps * 50);
    if (isIntersect){
        GeneratedRays gr = hit.getGeneratedRays();
        Material *mat = hit.getMaterial();

        tree.query(SPPMNode(hit.getCollision(), color, hit.getIncidenceNormal(), .0, .0, prob), c);   // 所以，这里不要计入材质颜色
        Vector3f textureCoef = Vector3f(1, 1, 1);
        if (hit.getTextureCoor() != nullptr && hit.getMaterial()->getTexture() != nullptr)
            textureCoef = hit.getMaterial()->getTexture()->
                GetSmoothColor(hit.getTextureCoor()->y(), hit.getTextureCoor()->x());

        double P;
        switch (mat->getProperty()){
            case DIFFUSE:
                light_pass(Ray(hit.getCollision(), Vector3f::randomRay(hit.getIncidenceNormal())), dep+1, 
                    color * mat->getDiffuseColor() * textureCoef, c, tree, group, prob);
                break;
            case SPECULAR:
                light_pass(gr.reflect, dep+1, 
                    color * specularShineness(mat->getSpecularColor(), textureCoef, mat->getShineness()), c, tree, group, prob);
                break;
            case TRANSPARENT:
                P = 0.25 + 0.5 * gr.reflectRatio;
                if (dep > 2){
                    if (erand48(X) < P)
                        light_pass(gr.reflect, dep+1, color * specularShineness(mat->getSpecularColor(), textureCoef, mat->getShineness()), c, tree, group, prob * gr.reflectRatio / P);
                    else
                        light_pass(gr.refract, dep+1, color * mat->getDiffuseColor() * textureCoef,
                            c, tree, group, prob * gr.refractRatio / (1 - P));
                }else{
                    light_pass(gr.reflect, dep+1, color * specularShineness(mat->getSpecularColor(), textureCoef, mat->getShineness()), c, tree, group, prob * gr.reflectRatio);
                    light_pass(gr.refract, dep+1, color * mat->getDiffuseColor() * textureCoef,
                        c, tree, group, prob * gr.refractRatio);
                }
        }
    }
}

#endif