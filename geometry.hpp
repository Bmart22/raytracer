//
//  geometry.hpp
//  
//
//  Created by Brendan Martin on 6/29/20.
//

#ifndef geometry_hpp
#define geometry_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include "variables.hpp"

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

struct Ray {
    vec3 origin;
    vec3 path;
};

struct Camera {
    vec3 position;
    vec3 direction;
    float focalLength;
};

struct Light {
    vec3 position;
    vec3 intensity;
};

class Material {
    vec3 diffuse;
    vec3 specular;
    float phongExp;
    vec3 reflectance;
    
public:
    Material();
    Material(vec3 diff, vec3 spec, float p, vec3 ref);
    void set(vec3 diff, vec3 spec, float p, vec3 ref);
    vec3 calcShading(vec3 normal, Light light, vec3 lightDir);
    vec3 getReflectance();
};

class Sphere {
    vec3 position;
    float radius;
    Material material;
public:
    Sphere();
    Sphere(vec3 pos, float rad, vec3 diff, vec3 spec, float p, vec3 ref);
    void set(vec3 pos, float rad, vec3 diff, vec3 spec, float p, vec3 ref);
    bool intersects(Ray ray, float &time, float minTime, float maxTime);
    bool intersects(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime);
    vec3 calcShading(vec3 normal, Light light, vec3 lightDir);
    vec3 getReflectance();
};

class Mesh {
    // Vertices are stored counterclockwise
    float vertices[9];
    Material material;
    
public:
    Mesh();
    Mesh(float verts[], vec3 diff, vec3 spec, float p, vec3 ref);
    void set(float verts[], vec3 diff, vec3 spec, float p, vec3 ref);
    vec3 getVertex( int ind );
    vec3 getNormal();
    bool intersects(Ray ray, float &time, float minTime, float maxTime);
    bool intersects(Ray ray, vec3 &location, vec3 &normal, float &time, float minTime, float maxTime);
    vec3 calcShading(vec3 normal, Light light, vec3 lightDir);
    vec3 getReflectance();
};



#endif /* geometry_hpp */
