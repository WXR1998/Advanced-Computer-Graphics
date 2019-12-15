#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>

extern unsigned short X[3];

class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual Ray generateJitterRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;    // 相机位置
    Vector3f direction; // 相机朝向
    Vector3f up;        // 相机成像平面的上
    Vector3f horizontal;// 相机成像平面的右
    // Intrinsic parameters
    int width;
    int height;
};

class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, double angle, double aperture) : Camera(center, direction, up, imgW, imgH), angle(angle), aperture(aperture) {
        // angle is in radian.
        f = 0.5 / std::tan(0.5 * angle);
    }

    /*
        生成有景深效果的相机光线
    */
    Ray generateJitterRay(const Vector2f &point) override {
        Vector3f x = horizontal.normalized() * (point.x() / width - 0.5);
        Vector3f y = up.normalized() * (point.y() / height - 0.5);
        Vector3f co = direction * f;
        Vector3f D = center + (co + x + y) * (center.length() / f);   // 此处假设成像平面为xOy平面
        Vector3f jitterCenter = center + (up * (erand48(X)-0.5) + horizontal * (erand48(X)-0.5)) * aperture / 100;
        return Ray(jitterCenter, (D - jitterCenter).normalized());
    }

    Ray generateRay(const Vector2f &point) override {
        Vector3f x = horizontal.normalized() * (point.x() / width - 0.5);
        Vector3f y = up.normalized() * (point.y() / height - 0.5);
        Vector3f co = direction * f;
        Vector3f D = center + co + x + y;
        return Ray(center, (D - center).normalized());
    }

private:
    double angle;
    double f;
    double aperture;
};

#endif //CAMERA_H
