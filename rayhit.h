/*
 * -----------------------------------------------------------------------------
 * Based on the "Ray Tracing in One Weekend" book series.
 * Authors: Peter Shirley, Steve Hollasch, Trevor David Black.
 * Source:  https://github.com/RayTracing/raytracing.github.io
 * License: CC0 1.0 Universal (Public Domain)
 * -----------------------------------------------------------------------------
 */

#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "material.h"

class hit_record {
  public:
    point3 p;
    vec3 normal;
    Material mat;
    double t;
    bool front_face;

    // Sets the hit record normal vector  
    void set_face_normal(const ray& r, const vec3& outward_normal) {
        
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif