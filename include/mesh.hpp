#ifndef MESH_H
#define MESH_H

#include <vector>
#include <cstdio>
#include "sphere.hpp"
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m, Vector3f bias = Vector3f::ZERO, double ratio = 1.0);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    std::vector<Vector3f> n;
    bool intersect(const Ray &r, Hit &h, double tmin) override;

private:

    // Normal can be used for light estimation
    void computeNormal();
    Sphere boundingSphere;
};

#endif
