#ifndef HIT_H
#define HIT_H

#include <iostream>
#include <vecmath.h>
#include "material.hpp"
#include "ray.hpp"
#define sqr(x) ((x)*(x))

class Material;

struct GeneratedRays{   // 反射折射光线
    Ray reflect, refract;
    double reflectRatio, refractRatio;
};

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        textureCoor = nullptr;
        t = 1e20;
    }

    Hit(double _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
        textureCoor = nullptr;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        textureCoor = h.textureCoor;
        collision = h.collision;
        incidence = h.incidence;
    }

    // destructor
    ~Hit(){
        if (textureCoor != nullptr)
            delete textureCoor;
        textureCoor = nullptr;
    }

    double getT() const {
        return t;
    }
    const Vector3f &getCollision() const{
        return collision;
    }

    Material *getMaterial() const {
        return material;
    }

    /*
        获得物体向外的法线
    */
    const Vector3f &getNormal() const {
        return normal.normalized();
    }

    /*
        获得和入射光线同侧的法线
    */
    const Vector3f getIncidenceNormal() const{
        if (Vector3f::dot(incidence, normal) > 0)
            return -normal.normalized();
        return normal.normalized();
    }

    /*
        得到碰撞点在物体上贴图的坐标
    */
    const Vector2f *getTextureCoor() const{
        return textureCoor;
    }

    /*
        根据碰撞点信息获得生成的反射、折射光线信息
    */
    GeneratedRays getGeneratedRays(){
        double relativeIota = Vector3f::dot(incidence, normal) < 0 ? 
            (1.0 / material->getIota()) : material->getIota();  // Iota_i / Iota_t
        Vector3f I = incidence.normalized();
        Vector3f N = getIncidenceNormal().normalized();
        Ray reflect(collision, (I - 2 * Vector3f::dot(I, N) * N).normalized());
        double costhetaI = fabs(Vector3f::dot(I, N));
        double costhetaT2 = 1 - sqr(relativeIota) * (1 - sqr(costhetaI));
        if (costhetaT2 <= 0)
            return (GeneratedRays){reflect, Ray(collision, Vector3f::ZERO), 1.0, 0.0};
        double costhetaT = sqrt(costhetaT2);

        double para, vert;
        para = (material->getIota() * costhetaI - costhetaT) / (material->getIota() * costhetaI + costhetaT);
        vert = (costhetaI - material->getIota() * costhetaT) / (costhetaI + material->getIota() * costhetaT);
        double kr = 0.5 * (sqr(para) + sqr(vert));
        double kt = 1.0 - kr;

        Ray refract(collision, (relativeIota * I + (relativeIota * (-Vector3f::dot(I, N)) - costhetaT) * N).normalized());
        return (GeneratedRays){reflect, refract, kr, kt};
    }

    /*
        double 距离 
        Material 入射点物体材质 
        Vector3f 法线方向 
        Vector3f 入射光线方向
        Vector3f 入射点 
        Vector2f* 材质贴图坐标 如果没有材质则为 nullptr
    */
    void set(double _t, Material *m, const Vector3f &n, const Vector3f &in, const Vector3f &coll, Vector2f *textureCoor_ = nullptr) {
        t = _t;
        material = m;
        normal = n;
        if (textureCoor != nullptr)
            delete textureCoor;
        textureCoor = textureCoor_;
        incidence = in;
        collision = coll;
    }

private:
    double t;
    Material *material; // 材质
    Vector3f normal;    // 法线方向
    Vector3f collision; // 碰撞点位置
    Vector2f *textureCoor;  // 材质贴图坐标
    Vector3f incidence; // 入射方向
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
