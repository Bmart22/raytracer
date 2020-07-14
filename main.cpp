//
//  main.cpp
//  
//
//  Created by Brendan Martin on 6/29/20.
//

#include <stdio.h>
#include <iostream>
#include <FreeImage.h>
#include <glm/glm.hpp>
#include "geometry.hpp"
#include "variables.hpp"

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

// screenHeight and screenWidth are expressed in pixels
float screenHeight = 500;
float screenWidth = 500;

struct Light lights[10];
int lightsUsed;

struct Camera cam = { vec3(0,5,0), vec3(0,-1,0), 1 };

//Sphere objects[10];
Mesh objects[10];
int numObjects;

Ray genCameraRay( int xCoor, int yCoor ) {
    float worldHeight = screenHeight / 100;
    float worldWidth = screenWidth / 100;
    
    vec3 right = glm::normalize( glm::cross( cam.direction, vec3(0.0,0.0,1.0) ) );
    vec3 up = glm::normalize( glm::cross( right, cam.direction ) );
    
    float u = (-worldWidth/2) + worldWidth*(xCoor+0.5)/screenWidth;
    float v = (-worldHeight/2) + worldHeight*(yCoor+0.5)/screenHeight;
    
    Ray camRay;
    camRay.origin = cam.position;
    // FreeImage begins at the lower left corner
    camRay.path = (cam.focalLength * glm::normalize(cam.direction)) + (u*right) + (v*up);
    //std::cout << glm::normalize(cam.direction).y;
    
    return camRay;
}

vec3 raytrace( Ray ray ) {
    
    vec3 color = vec3(0,0,0);
    
    vec3 realLocation = vec3(0,0,0);
    vec3 location = vec3(0,0,0);
    
    vec3 realNormal = vec3(0,0,0);;
    vec3 normal = vec3(0,0,0);
    
    float realTime = std::numeric_limits<float>::infinity();
    float time = 0;
    
    int closestObj = -1;
    
    // Loop over every object
    for (int obj = 0; obj < numObjects; obj++) {
        // If the current object is intersected
        if (objects[obj].intersects(ray, location, normal, time)) {
            // If no object has been intersected before or current object is closer
            // than the previous closest object
            if (time < realTime) {
                // Update the values corresponding to the closest object
                realTime = time;
                realLocation = location;
                realNormal = normal;
                closestObj = obj;
            }
        }
    }
    if (closestObj != -1) {
        color = objects[closestObj].calcShading(realNormal, realLocation, lights, lightsUsed, objects, numObjects);
    }
    
    return color;
}

int main(int argc, char* argv[]) {
    lights[0].position = vec3(5,5,0);
    lights[0].intensity = vec3(1,1,1);
    lights[1].position = vec3(0,5,0);
    lights[1].intensity = vec3(0.5,0.5,0.5);
    lightsUsed = 2;
    
//    objects[1].set(vec3(3,0,0), 3, vec3(200,0,0), vec3(100,100,100), 100);
//    objects[0].set(vec3(0,1,0), 3, vec3(200,0,0), vec3(100,100,100), 100);
    
    float verts[9] = {0,0,4, 4,0,-4, -4,0,-4};
    objects[0].set(verts, vec3(200,0,0), vec3(100,100,100), 100);
    float verts2[9] = {2,3,4, 2,3,-4, 1,0,0};
    objects[1].set(verts2, vec3(200,0,0), vec3(100,100,100), 100);
    numObjects = 2;

    FreeImage_Initialise();

    int bitsPerPixel = 24;
    FIBITMAP* bitmap = FreeImage_Allocate(screenWidth, screenHeight, bitsPerPixel);
    
    RGBQUAD color;
    for (int i = 0; i < screenWidth; i++) {
        for (int j = 0; j < screenHeight; j++) {
            vec3 colVec = raytrace( genCameraRay(i,j) );
            color.rgbRed = colVec.z;
            color.rgbGreen = colVec.y;
            color.rgbBlue = colVec.x;
            FreeImage_SetPixelColor(bitmap,i,j,&color);
        }
    }
    
    FreeImage_Save(FIF_PNG, bitmap, "image.png", 0);
    FreeImage_DeInitialise();
}
