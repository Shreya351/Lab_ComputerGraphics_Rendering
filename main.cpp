/*
 * -----------------------------------------------------------------------------
 * Based on the "Ray Tracing in One Weekend" book series.
 * Authors: Peter Shirley, Steve Hollasch, Trevor David Black.
 * Source:  https://github.com/RayTracing/raytracing.github.io
 * License: CC0 1.0 Universal (Public Domain)
 * -----------------------------------------------------------------------------
 */

#include <iostream>
#include "vector.h"
#include "color.h"
#include "ray.h"
#include "material.h"
#include "rayhit.h"
#include "sphere.h"
#include "hittable_list.h"
#include "mesh.h"


const double infinity = std::numeric_limits<double>::infinity();

vec3 reflect(const vec3& ref_vec, const vec3& normal) {
    return ref_vec - 2 * dot(ref_vec, normal) * normal;
}

int main() {

    // Image
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 1024;
    const int samples_per_pixel = 100;
    
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    hittable_list world;

    world.add(make_shared<sphere>(point3(-1.0,1.4,-2.0), 0.8, Material::Red()));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100, Material::Yellow()));

    auto bunny_mesh = make_shared<mesh>(Material::Green());
    std::cerr << "About to load bunny.obj..." << std::endl;
    load_object("bunny.obj", bunny_mesh.get());
    std::cerr << "After load, triangles: " << bunny_mesh->triangles.size() << std::endl;
    bunny_mesh->bounding_box();

    auto teapot_mesh = make_shared<mesh>(Material::Green());
    std::cerr << "About to load teapot.obj..." << std::endl;
    load_object("teapot.obj", teapot_mesh.get());
    std::cerr << "After load, triangles: " << teapot_mesh->triangles.size() << std::endl;
    teapot_mesh->bounding_box();

    world.add(bunny_mesh);
    world.add(teapot_mesh);

    // Camera   
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width)/image_height);
    auto camera_center = point3(0.0, 1.2, 2);
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;
    auto viewport_upper_left = camera_center
                             - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Light position
    vec3 light_position(5.0, 5.0, 0.0);
    //vec3 light_position(0.0, 5.0, -1.0);
    
    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        
        
        for (int i = 0; i < image_width; i++) {

            color pixel_color(0, 0, 0);

            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            // Check for intersection
            hit_record rec;
            color sample_color(0.8, 0.9, 1.0); // Default to black (background)
            
            if (world.hit(r, 0.001, infinity, rec)) {// Light is hitting an object
                
                // Shadow ray
                vec3 light_dir = (light_position - rec.p).normalize();
                ray shadow_ray(rec.p, light_dir);
                hit_record shadow_rec;  
                double light_distance = (light_position - rec.p).length();
                bool is_in_shadow = world.hit(shadow_ray, 0.001, light_distance, shadow_rec);
                
                // Shading calculations
                vec3 N = rec.normal.normalize();
                vec3 L = light_dir;
                vec3 V = (camera_center - rec.p).normalize();
                vec3 H = (L + V).normalize();
                vec3 light_color(1.0, 1.0, 1.0);

                vec3 ambient_color = rec.mat.ambient; 
                vec3 diffuse_color(0.0, 0.0, 0.0);
                vec3 specular_color(0.0, 0.0, 0.0);

                if (!is_in_shadow) {
                    double diffuseFactor = std::max(0.0, dot(N, L));
                    diffuse_color = rec.mat.diffuse * diffuseFactor * light_color;

                    if (diffuseFactor > 0.0) { 
                        double specular_factor = std::pow(std::max(0.0, dot(H, N)), (double)rec.mat.shininess);
                        specular_color = rec.mat.specular * specular_factor * light_color;
                    }
                }
                
                sample_color = ambient_color + diffuse_color + specular_color;
            }
            
            pixel_color += sample_color;
            write_color(std::cout, pixel_color);
        }
        
    }

    std::clog << "\rDone.                 \n";
}


// color ray_color(const ray& r, const hittable& world, int depth) {
//     hit_record rec;
//     if (depth <= 0) {
//         return color(0, 0, 0);
//     }

//     if (world.hit(r, 0.001, infinity, rec)) {
//         vec3 target = reflect(unit_vector(r.direction()), rec.normal); // Reflection direction
//         ray reflected_ray(rec.p, target);
//         color reflected_color = ray_color(reflected_ray, world, depth - 1);

//         // Combine the reflected color with the object's base color
//         return 0.5 * reflected_color + 0.5 * color(1, 1, 0); // Yellow base color
//     }

//     // Background color (black)
//     return color(0, 0, 0);
// }

// Shading Function (Blinn-Phong) ---
// vec3 shade(const sphere& sphere, const vec3& hitPoint, const ray& ray, const vec3& lightPos) {
//     Material mat = sphere.mat;
//     vec3 light_color(1.0f, 1.0f, 1.0f); // White light

//     vec3 N = (hitPoint - sphere.center).normalize(); // Normal
//     vec3 L = (lightPos - hitPoint).normalize();     // Light Direction
//     vec3 V = (ray.origin() - hitPoint).normalize();  // View Direction
//     vec3 H = (L + V).normalize();

//     // Ambient
//     vec3 ambient_color = mat.ambient; 

//     // Diffuse
//     double diffuseFactor = std::max(0.0, dot(N,L));
//     vec3 diffuse_color = mat.diffuse * diffuseFactor * light_color;

//     // Specular
//     double specular_factor = 0.0;
//     if (diffuseFactor > 0.0) { 
//         specular_factor = std::pow(std::max(0.0, dot(H,L)), mat.shininess);
//     }
//     vec3 specular_color = mat.specular * specular_factor * light_color;

//     return ambient_color + diffuse_color + specular_color;
// }