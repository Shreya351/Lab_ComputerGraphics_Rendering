/*
 * -----------------------------------------------------------------------------
 * Taken help from "Ray Tracing in One Weekend" book series.
 * Authors: Peter Shirley, Steve Hollasch, Trevor David Black.
 * Source:  https://github.com/RayTracing/raytracing.github.io
 * License: CC0 1.0 Universal (Public Domain)
 * -----------------------------------------------------------------------------
 */

#include <fstream>
#include <sstream>
#include <vector>  
#include <string>
#include <iostream>
#include "vector.h"
#include "rayhit.h"
#include "material.h"

// Triangle struct with 3 vertices and a normal vector
struct Triangle {
    vec3 v0, v1, v2;
    vec3 normal;
};

const double inf = std::numeric_limits<double>::infinity();

//  Mesh class holds a 3D shape made of triangles
class mesh: public hittable {
    public: 


    std::vector<Triangle> triangles;
    Material mat;
    vec3 bb_min;
    vec3 bb_max;
    bool has_bb;

    mesh(const Material &m) : mat(m) , has_bb(false){}

    ~mesh() override = default;

    // Checks if the ray intersects the bounding box
    bool intersect_bb(const ray& r, double t_min, double t_max) const {
        for(int axis=0; axis<3; axis++){
            double adinv = 1.0 / r.direction()[axis];
            auto t0 = (bb_min[axis] - r.origin()[axis]) * adinv;
            auto t1 = (bb_max[axis] - r.origin()[axis]) * adinv; 
        
            if(adinv < 0.0)
                std::swap(t0, t1);
            
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            
            if (t_max <= t_min)
                return false;
        
        }
        return true;    
            
    }

   // This function checks if we hit the object
    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override{
        double closest_t = ray_tmax;
        bool hit_anything = false;
        vec3 hit_normal;
        
        if (has_bb && !intersect_bb(r, ray_tmin, ray_tmax)) {
            return false;
        }

        // Pass through all triangles and find the closest intersection    
        for (const auto& tri: triangles){
            double i = intersect_triangle(r.origin(), r.direction(), tri);
            if (i > ray_tmin && i < closest_t){
                closest_t = i;
                hit_anything = true;
                hit_normal = tri.normal;

            }
        }

        if (hit_anything){
            rec.t = closest_t;
            rec.p = r.at(closest_t);
            rec.mat = mat;
            rec.set_face_normal(r, hit_normal);
            // rec.normal = hit_normal;
            return true;
        }

        return false;
    }

    // Computes bounding box
    void bounding_box(){
        if (triangles.empty()){
            has_bb = false;
            return;
        }
        
        bb_min = vec3(inf, inf, inf);
        bb_max = vec3(-inf, -inf, -inf);

        for (auto& tri :triangles){
            vec3 verts[3] = {tri.v0, tri.v1, tri.v2};

            for (int i = 0; i < 3; i++){
                const vec3& v = verts[i];
                bb_min = vec3(
                    std::min(bb_min.x(), v.x()),
                    std::min(bb_min.y(), v.y()),
                    std::min(bb_min.z(), v.z())
                );
                bb_max = vec3(
                    std::max(bb_max.x(), v.x()),
                    std::max(bb_max.y(), v.y()),
                    std::max(bb_max.z(), v.z())
                );
            }
        }
        has_bb = true;
    } 

    private:

    // Moller-Trumbore algorithm for ray-triangle intersection in 3D
    // Will return the distance to where the ray hits  
    double intersect_triangle(const vec3& origin, const vec3& direction, Triangle tri) const {
        const double epsilon = 0.0001;

        vec3 edge1 = tri.v1 - tri.v0;
        vec3 edge2 = tri.v2 - tri.v0;
        vec3 P = cross(direction, edge2);

        double det = dot(edge1, P);

        if(det > -epsilon && det < epsilon)
            return -1.0;

        double inv_det = 1.0 / det;
        vec3 T = origin - tri.v0;
   
        // a and b are the barycentric coordinates that are used to determine the point where the ray hit
        double a = dot(T, P) * inv_det;
        
        if(a < 0.0 || a > 1.0)
            return -1.0;

        vec3 q = cross(T, edge1);

        double b = dot(direction, q) * inv_det;
        
        if(b < 0.0 || a + b > 1.0)
            return -1.0;

        double p = dot(edge2, q) * inv_det;
        if(p > epsilon)
            return p;

        return -1.0; // if it doesn't hit then return -1.0 

    }    

};    
    
inline void load_object(const std::string& filename, mesh* mesh_obj) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "FILE NOT FOUND" << filename << std::endl;
        return;
    }

    std::vector<vec3> vertices;
    std::string line;

    while (std::getline(file, line)){
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            double x , y, z;
            ss >> x >> y >> z;

            //vertices.push_back(Vec3(x * 5.0, (y * 5.0) - 1.0, z * 5.0));
            // vertices.push_back(vec3(x * 3.0, y * 1.1 + 2.7, z * 1.1 - 1.0)); 
            // vertices.push_back(vec3(x * 1.1 + 0.4, y * 1.1 - 0.7, z * 1.1 - 1.0)); // Bunny
           // vertices.push_back(vec3(x * 0.5, y * 0.5 - 0.5, z * 0.5 - 2.5)); // Teapot
            
            if (filename.find("bunny") != std::string::npos) {
                vertices.push_back(vec3(x * 2.5 - 1.0, y * 1.1 - 0.7, z * 2.5 - 1.5));
            }
            else if (filename.find("teapot") != std::string::npos) {
                vertices.push_back(vec3(x * 0.5 + 1.5, y * 0.5 - 0.5, z * 0.5 - 2.0));
            }
            else {
                vertices.push_back(vec3(x, y, z));
            }
        
        }
        else if (type == "f"){
            int vertex_index[3];
            for (int i = 0; i < 3; ++i){
                std::string segment;
                ss >> segment;

                size_t slash = segment.find('/');
                int index = std::stoi(segment.substr(0, slash));
                vertex_index[i] = index - 1; // IMP consider OBJ file index
            }

            Triangle triangle;
            triangle.v0 = vertices[vertex_index[0]];
            triangle.v1 = vertices[vertex_index[1]];
            triangle.v2 = vertices[vertex_index[2]];

            vec3 edge1 = triangle.v1 - triangle.v0;
            vec3 edge2 = triangle.v2 - triangle.v0;
            // vec3 edge2 = triangle.v2 - triangle.v1;
            triangle.normal = unit_vector(cross(edge1, edge2));
            //triangle.normal = unit_vector(cross(edge2, edge1));
            mesh_obj->triangles.push_back(triangle);
        }
    }
    std::cerr << "Total: " << mesh_obj->triangles.size() << " triangles" << std::endl;
    // std::cout << "Total: ": " << mesh_obj->triangles.size() << " triangles" << std::endl;
}    