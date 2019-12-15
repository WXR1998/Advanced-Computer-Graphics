#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

extern bool debug;
class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = *(new Vector3f(0, 0, 0));
        radius = 1.0;
    }

    Sphere(const Vector3f &center, double radius, Material *material) : 
        Object3D(material), center(center), radius(radius){
    }

    ~Sphere() override = default;

    Vector3f getD(const Ray &r){
        // 取得D点坐标，D是圆心C到直线r的垂足
        Vector3f oc = center - r.getOrigin();
        Vector3f dir = r.getDirection();
        Vector3f od = Vector3f::dot(oc, dir) * dir;
        return r.getOrigin() + od;
    }

    /*
        给定一个球上的点a，求其在texture上的经纬度坐标
    */
    Vector2f getTextureCoor(Vector3f a){
        a = a - center;
        double theta = atan2(a.z(), a.x());
		if (theta < 0)
			theta += 2 * M_PI;
        double t = a.y();
        return Vector2f(theta, t);
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        // Ray r是摄像机发出的射线，Hit h是之前所有处理过的交点中离摄像机最近的
        Vector3f D = getD(r);
        Vector3f cd = D - center;
        if (cd.length() > radius)
            return false;
        else{
            bool flag = false;
            Vector3f da = r.getDirection() * std::sqrt(radius * radius - cd.length() * cd.length());
            Vector3f db = -da;
            Vector3f A = D + da;
            Vector3f B = D + db;
            Vector3f oa = A - r.getOrigin();
            Vector3f ob = B - r.getOrigin();
            if (oa.length() >= tmin && Vector3f::dot(oa, r.getDirection()) >= 0)
                if (oa.length() < h.getT()){
                    Vector2f tmp = getTextureCoor(A);
                    h.set(oa.length(), 
                        material, 
                        (A - center).normalized(), 
                        r.getDirection(), 
                        A, 
                        new Vector2f(tmp.x(), tmp.y()));
                    flag = true;
                }
            if (ob.length() >= tmin && Vector3f::dot(ob, r.getDirection()) >= 0)
                if (ob.length() < h.getT()){
                    Vector2f tmp = getTextureCoor(B);
                    h.set(ob.length(), 
                        material, 
                        (B - center).normalized(), 
                        r.getDirection(), 
                        B, 
                        new Vector2f(tmp.x(), tmp.y()));
                    flag = true;
                }
            return flag;
        }
    }

protected:
    Vector3f center;
    double radius;

};


#endif
