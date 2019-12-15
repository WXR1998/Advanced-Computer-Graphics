#ifndef MESH_H
#define MESH_H

#include <vector>
#include <cstdio>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);

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

    // void drawGL() override {
    //     for (std::vector<TriangleIndex>::iterator i = t.begin(); i != t.end(); i++){
    //         Triangle tri(v[(*i)[0]], v[(*i)[1]], v[(*i)[2]], this->material);
    //         tri.drawGL();
    //     }
    // }

private:

    // Normal can be used for light estimation
    void computeNormal();
};

#endif
