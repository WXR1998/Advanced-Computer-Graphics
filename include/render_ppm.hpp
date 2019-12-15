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

void camera_pass(Ray ray, int dep, int index, std::vector<SPPMNode> &sightpoint){

}

void light_pass(){

}

#endif