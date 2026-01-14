/*
 * -----------------------------------------------------------------------------
 * Based on the "Ray Tracing in One Weekend" book series.
 * Authors: Peter Shirley, Steve Hollasch, Trevor David Black.
 * Source:  https://github.com/RayTracing/raytracing.github.io
 * License: CC0 1.0 Universal (Public Domain)
 * -----------------------------------------------------------------------------
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    double shininess;

    // Highly reflective Gold material for the large sphere
    static Material Gold() {
        return {
            {0.05f, 0.05f, 0.0f},  // Ambient (subtle baseline color)
            {0.8f, 0.6f, 0.0f},    // Diffuse (main color)
            {1.0f, 1.0f, 1.0f},    // Specular (white highlight for max shine)
            120.0f                 // High shininess for sharp specular reflection
        };
    }

    static Material Red() {
        return {
            {0.1f, 0.0f, 0.0f},    // Ambient (dark red baseline)
            {0.8f, 0.0f, 0.0f},    // Diffuse (bright red main color)
            {1.0f, 1.0f, 1.0f},    // Specular (white highlight)
            32.0f                  // Shininess
        };
    }
    
    // Green material 
    static Material Green() {
        // return {
        //     {0.0f, 0.05f, 0.0f},
        //     {0.0f, 0.6f, 0.0f},
        //     {0.4f, 0.4f, 0.4f},
        //     30.0f
        // };
        return {
            {0.0f, 0.1f, 0.0f},    // Ambient - slightly brighter
            {0.0f, 0.8f, 0.0f},    // Diffuse - brighter green
            {1.0f, 1.0f, 1.0f},    // Specular - full white for shine
            50.0f                   // Shininess
        };
    }

    // Yellow material
    static Material Yellow() {
        // return {
        //     {0.1f, 0.1f, 0.0f},
        //     {1.0f, 1.0f, 0.0f},
        //     {0.5f, 0.5f, 0.5f},
        //     10.0f
        // };
        return {
            {0.1f, 0.1f, 0.0f},    // Ambient
            {0.9f, 0.9f, 0.2f},    // Diffuse - brighter yellow
            {0.8f, 0.8f, 0.8f},    // Specular
            20.0f                   // Shininess
        };
    }
};

#endif