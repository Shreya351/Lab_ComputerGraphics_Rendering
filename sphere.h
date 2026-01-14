/*
 * -----------------------------------------------------------------------------
 * Based on the "Ray Tracing in One Weekend" book series.
 * Authors: Peter Shirley, Steve Hollasch, Trevor David Black.
 * Source:  https://github.com/RayTracing/raytracing.github.io
 * License: CC0 1.0 Universal (Public Domain)
 * -----------------------------------------------------------------------------
 */

#ifndef SPHERE_H
#define SPHERE_H

#include "rayhit.h" 
#include "vector.h"
#include "material.h"

class sphere : public hittable {
    public:
    sphere(const point3& center, double radius, Material material) : center(center), radius(std::fmax(0,radius)), mat(material) {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (root <= ray_tmin || ray_tmax <= root) {
            root = (h + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;
        
        return true;
    }

    Material mat;
    point3 center;
    double radius;
};

#endif