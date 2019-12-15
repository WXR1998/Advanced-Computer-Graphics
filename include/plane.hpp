#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

extern const double eps;

class Plane : public Object3D {
public:
    Plane() {
        d = 0;
        normal = Vector3f(0, 1, 0);
        X = Vector3f(1, 0, 0);
        Y = Vector3f(0, 0, 1);
    }

    Plane(const Vector3f &normal, double d, Material *m) : 
        Object3D(m), normal(normal), d(d) {
        if (fabs(normal.y()) > eps || fabs(normal.z()) > eps)
            Y = Vector3f(0, normal.z(), -normal.y()).normalized();
        else
            Y = Vector3f(0, 0, 1).normalized();
        X = Vector3f::cross(normal, Y).normalized();
    }

    ~Plane() override = default;

    Vector2f getTextureCoor(Vector3f a){
        Vector3f center = Ray(Vector3f(0, 0, 0), (-normal).normalized()).pointAtParameter(d);
        // if (Vector3f::dot(center, normal) + d != 0)
        //     printf("%lf\n", Vector3f::dot(center, normal) + d);
        double A, B;
        if (fabs(Y.x() * X.y() - Y.y() * X.x()) > eps)
            B = (a.x() * X.y() - a.y() * X.x()) / (Y.x() * X.y() - Y.y() * X.x());
        else if (fabs(Y.x() * X.z() - Y.z() * X.x()) > eps)
            B = (a.x() * X.z() - a.z() * X.x()) / (Y.x() * X.z() - Y.z() * X.x());
        else if (fabs(Y.y() * X.z() - Y.z() * X.y()) > eps)
            B = (a.y() * X.z() - a.z() * X.y()) / (Y.y() * X.z() - Y.z() * X.y());
        else{
            printf("Error in getTextureCoor\n");
            exit(-1);
        }
        
        if (fabs(X.x()) > eps)
            A = (a.x() - B*Y.x()) / X.x();
        else if (fabs(X.y()) > eps)
            A = (a.y() - B*Y.y()) / X.y();
        else if (fabs(X.z()) > eps)
            A = (a.z() - B*Y.z()) / X.z();
        else{
            printf("Error in getTextureCoor\n");
            exit(-1);
        }
        return Vector2f(A, B);
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        if (std::fabs(Vector3f::dot(r.getDirection(), normal)) < eps)
            return false;
        double s = - (Vector3f::dot(r.getOrigin(), normal) - d) 
            / (Vector3f::dot(normal, r.getDirection()));
        Vector3f P = r.pointAtParameter(s);
        Vector3f op = P - r.getOrigin();
        if (op.length() >= tmin && Vector3f::dot(op, r.getDirection()) >= 0)
            if (op.length() < h.getT()){
                Vector2f tmp = getTextureCoor(P);
                h.set(op.length(), 
                    material, 
                    normal, 
                    r.getDirection(),
                    P,
                    new Vector2f(tmp.x(), tmp.y()));
                return true;
            }
        return false;
    }

protected:
    Vector3f normal;

    // 平面上的二维坐标系
    Vector3f X, Y;
    double d;
};

#endif //PLANE_H
		

