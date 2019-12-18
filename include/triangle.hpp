#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;

class Triangle: public Object3D
{

public:
    Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m){
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		Vector3f ab = b - a;
		Vector3f ac = c - a;
		normal = Vector3f::cross(ab, ac).normalized();
	}

	bool intersect(const Ray& r, Hit &h, double tmin) override {
        if (std::fabs(Vector3f::dot(r.getDirection(), normal)) < 1e-8)
            return false;
        double s = (Vector3f::dot(normal, vertices[0] - r.getOrigin()))
            / (Vector3f::dot(normal, r.getDirection()));
        Vector3f P = r.pointAtParameter(s);
		Vector3f ab = vertices[1] - vertices[0],
				 bc = vertices[2] - vertices[1],
				 ca = vertices[0] - vertices[2];
		if (Vector3f::dot(Vector3f::cross(ab, P - vertices[0]), normal) >= 0 &&
			Vector3f::dot(Vector3f::cross(bc, P - vertices[1]), normal) >= 0 &&
			Vector3f::dot(Vector3f::cross(ca, P - vertices[2]), normal) >= 0) {
			Vector3f op = P - r.getOrigin();
			if (op.length() >= tmin && Vector3f::dot(op, r.getDirection()) >= 0)
				if (op.length() < h.getT()){
					h.set(op.length(), 
						material, 
						Vector3f::dot(r.getDirection(), normal) <= 0 ? normal : -normal,
						r.getDirection(),
						P, 
						nullptr);
					return true;
				}
		}
		return false;
	}
	Vector3f normal;
	Vector3f vertices[3];

    // void drawGL() override {
    //     Object3D::drawGL();
    //     glBegin(GL_TRIANGLES);
    //     glNormal3fv(normal);
    //     glVertex3fv(vertices[0]); glVertex3fv(vertices[1]); glVertex3fv(vertices[2]);
    //     glEnd();
    // }

protected:
};

#endif //TRIANGLE_H
