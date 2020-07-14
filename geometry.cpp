//
//  geometry.cpp
//  
//
//  Created by Brendan Martin on 6/29/20.
//

#include <iostream>
#include "geometry.hpp"

// Sphere Class

// Default constructor
Sphere::Sphere() {
    position = vec3(0.0f);
    radius = 0;
    diffuse = vec3(0.0f);
    specular = vec3(0.0f);
    phongExp = 0;
}

// Constructor for the Sphere class
Sphere::Sphere(vec3 pos, float rad, vec3 diff, vec3 spec, float p) {
    position = pos;
    radius = rad;
    diffuse = diff;
    specular = spec;
    phongExp = p;
}

void Sphere::set(vec3 pos, float rad, vec3 diff, vec3 spec, float p) {
    position = pos;
    radius = rad;
    diffuse = diff;
    specular = spec;
    phongExp = p;
}
    
// Color and normal passed by reference
// Determine if ray hits sphere, update color and normal
// Return boolean value indicating if the sphere is hit
bool Sphere::intersects(Ray ray, vec3 &location, vec3 &normal, float &time) {
    bool isHit = false;
    time = 0.0;
    
    // Origin minus position (OMP)
    vec3 OMP = ray.origin - position;
    float path_2 = glm::dot(ray.path, ray.path);
    
    // Calculate the discriminant
    float discriminant = glm::dot(ray.path, OMP)*glm::dot(ray.path, OMP) - path_2 * (glm::dot(OMP,OMP) - (radius*radius));
    
    // If the dicriminant is less than 0, the ray does not intersect the sphere
    if (discriminant < 0.0) {
        isHit = false;
    }
    // Otherwise, the ray intersects the sphere at least once.
    else{
        // Find the time value when discriminant = 0 (tangential intersection)
        isHit = true;
        time = glm::dot(-ray.path, OMP) / path_2;
        
        // If the discriminant is non-zero, there are two intersections
        // Find the smaller of the two time values
        if (discriminant > 0.0) {
            time = time - (glm::sqrt(discriminant)/path_2);
        }
        
        // Update the parameters
        location = ray.origin + (time * ray.path);
        normal = glm::normalize( location - position );
    }
    return isHit;
}

vec3 Sphere::calcShading(vec3 normal, vec3 location, struct Light lights[], int lightsUsed, Sphere objects[], int numObjects) {
    
    vec3 total = vec3(0.1f);
    
    for (int i = 0; i < lightsUsed; i++) {
        //Calculate diffuse component
        vec3 lightDir = glm::normalize(lights[i].position-location);
        float LdotN = glm::dot(lightDir, normal);
        total += diffuse * lights[i].intensity * glm::max(LdotN, 0.0f);
        
        //Calculate specular component
        vec3 halfvec = glm::normalize(lightDir + normal);
        float NdotH = glm::dot(normal, halfvec);
        total += specular * lights[i].intensity * glm::pow( glm::max(NdotH, 0.0f), phongExp );
        
    }
    
    return glm::min( total, vec3(255,255,255) );
}

// Mesh Class

// Default constructor
Mesh::Mesh() {
    for (int i = 0; i < 9; i++) {
        vertices[i] = 0;
    }
}

// Vertices are stored counterclockwise
Mesh::Mesh(float verts[], vec3 diff, vec3 spec, float p) {
    for (int i = 0; i < 9; i++) {
        vertices[i] = verts[i];
    }
    material.set(vec3 diff, vec3 spec, float p);
}

void Mesh::set(float verts[], vec3 diff, vec3 spec, float p) {
    for (int i = 0; i < 9; i++) {
        vertices[i] = verts[i];
    }
    material.set(vec3 diff, vec3 spec, float p);
}

vec3 Mesh::getVertex( int ind ) {
    if (ind >= 0 && ind < 3) {
        return vec3( vertices[(ind*3)+0], vertices[(ind*3)+1], vertices[(ind*3)+2] );
    }
    return vec3(0,0,0);
}

vec3 Mesh::getNormal() {
    vec3 edge1 = getVertex(1) - getVertex(0);
    vec3 edge2 = getVertex(2) - getVertex(0);
    return glm::normalize( glm::cross(edge1, edge2) );
}

bool Mesh::intersects(Ray ray, float &time, float minTime, float maxTime) {
    vec3 edge_ba = getVertex(0) - getVertex(1);
    vec3 edge_ca = getVertex(0) - getVertex(2);
    vec3 aMinusOrigin = getVertex(0) - ray.origin;
    
    float ei_hf = edge_ca[1] * ray.path[2] - ray.path[1] * edge_ca[2];
    float gf_di = -(edge_ca[0] * ray.path[2] - ray.path[0] * edge_ca[2]);
    float dh_eg = edge_ca[0] * ray.path[1] - ray.path[0] * edge_ca[1];
    
    float M = edge_ba[0] * ei_hf + edge_ba[1] * gf_di + edge_ba[2] * dh_eg;
    
    
    float beta = (aMinusOrigin[0] * ei_hf + aMinusOrigin[1] * gf_di + aMinusOrigin[2] * dh_eg) / M;
    
    // Condition for early termination
    if (beta < 0 || beta > 1) {
        return false;
    }
    
    float ak_jb = edge_ba[0] * aMinusOrigin[1] - aMinusOrigin[0] * edge_ba[1];
    float jc_al = -(edge_ba[0] * aMinusOrigin[2] - aMinusOrigin[0] * edge_ba[2]);
    float bl_kc = edge_ba[1] * aMinusOrigin[2] - aMinusOrigin[1] * edge_ba[2];
    
    float gamma = (ray.path[2] * ak_jb + ray.path[1] * jc_al + ray.path[0] * bl_kc) / M;
    
    // Condition for early termination
    if (gamma < 0 || gamma > 1-beta) {
        return false;
    }
    
    // If we have gotten this far, the ray has hit the triangle
    // Calculate necessary values
    float t = -(edge_ca[2] * ak_jb + edge_ca[1] * jc_al + edge_ca[0] * bl_kc) / M;
    if (t > minTime && t < maxTime) {
        time = t;
        return true;
    }
    
    return false;
}

bool Mesh::intersects(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime) {
    bool success = intersects(ray, time, minTime, maxTime);
    
    if (success) {
        location = ray.origin + (time * ray.path);
        normal = getNormal();
    }
    
    return success;
}

vec3 Mesh::calcShading(vec3 normal, vec3 location, struct Light lights[], int lightsUsed, Mesh objects[], int numObjects) {
    
    return material.calcShading(normal, location, lights, lightsUsed, objects[], numObjects);
}

// Material Class

Material::Material() {
    diffuse = vec3(0.0f);
    specular = vec3(0.0f);
    phongExp = 0;
}

Material::Material(vec3 diff, vec3 spec, float p) {
    diffuse = diff;
    specular = spec;
    phongExp = p;
}

void Material::set(vec3 diff, vec3 spec, float p) {
    diffuse = diff;
    specular = spec;
    phongExp = p;
}

vec3 Material::calcShading(vec3 normal, vec3 location, struct Light lights[], int lightsUsed, Mesh objects[], int numObjects) {
    
    // Dummy variable to pass to the intersects function in place of time
    float dummy;
    
    vec3 total = vec3(0.1f);
    bool inShadow;
    
    for (int i = 0; i < lightsUsed; i++) {
        vec3 lightDir = glm::normalize(lights[i].position-location);
        
        //Test to see if any object blocks the light
        Ray shadowRay = {location,lightDir};
        inShadow = false;
        int o = 0;
        while (o < numObjects && inShadow == false) {
            inShadow = objects[o].intersects(shadowRay, dummy);
            o++;
        }
        // If the object is not in shadow, calculate the lighting
        if (inShadow == false) {
            //Calculate diffuse component
            float LdotN = glm::dot(lightDir, normal);
            total += diffuse * lights[i].intensity * glm::max(LdotN, 0.0f);
            
            //Calculate specular component
            vec3 halfvec = glm::normalize(lightDir + normal);
            float NdotH = glm::dot(normal, halfvec);
            total += specular * lights[i].intensity * glm::pow( glm::max(NdotH, 0.0f), phongExp );
        }
    }
    
    return glm::min( total, vec3(255,255,255) );
}
