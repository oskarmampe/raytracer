#include "raytracer_io.h"
#include "geometry.h"

/**
 * 
 * Simple implementation of a ray tracer.
 *  
*/

// Image initialization
int width = 128, height = 128;
double invWidth = 1.0/128.0, invHeight = 1.0/128.0;
double fov = 90.0; 
double scale = tan((fov * 0.5)*M_PI/180.0f);
double aspect = width/height;

Vec3 eye(0, 0, 0);
Vec3 lookDirection(0, 0, 1);
Vec3 upVector(0, 1, 0);

// Light initialization
//POSITION, COLOUR
LightStruct light = {Vec3(0,0.1,0), Vec3(1,1,1)};

// Shader initialization
PhongShader shader;

// Triangle initialization
Vec3 vertices[3] = {Vec3(-0.04688, -0.84375, 1), Vec3(0.5625, 0.5625, 1), Vec3(-0.60938, 0.40625, 1)};
//Vec3 vertices[3] = {Vec3(61, 10, 1), Vec3(100, 100, 1), Vec3(25, 90, 1)};
Vec3 c[3] = {Vec3(255, 0, 0), Vec3(0, 255, 0), Vec3(0, 0, 255)};
//Vec3 ambient(1.0f, 0.5f, 0.31f), diffuse(1.0f, 0.5f, 0.31f), specular(0.5f, 0.5f, 0.5f);
double ambient = 0.1f, diffuse = 0.5f, specular = 0.4f; 
double specular_coeff = 16.0f;
Triangle tri = Triangle(vertices, c, ambient, diffuse, specular, specular_coeff);

/*
for each pixel p with coords (i,j)
let R_ij be the ray from eye through p
cast ray R into scene
find point P where R first intersects the plane
use half-plane test to check whether P is inside the triangle
calculate colour and store in image
*/
void partA()
{
    std::ofstream file("intersections.ppm");
    write_ppm_header(&file);

    // Loop through each pixel 
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            // The ray has to go through the middle of the pixel, so an offset is needed
            float x = (2 * ((i + 0.5) * invWidth) - 1); 
            float y = (1 - 2 * ((j + 0.5) * invHeight)); 

            // Create a direction vector i.e. direction of ray.
            Vec3 dir(x, y, lookDirection.z); 
            dir.normalize(); 
            float t, alpha, beta, gamma;// Needed later for barycentric interpolation
            Vec3 fragment(0, 0, 0); // Needed later for shading
            Vec3 normal(0, 0, 0); // Needed later for shading
            if (tri.intersect(eye, dir, t, alpha, beta, gamma, normal, fragment))
            {
                file << 0 << " " << 0 << " " << 0 << " ";
            }
            else
            {
                file << 255 << " " << 255 << " " << 192 << " ";
            }
        }
    }
    file.close(); 
}

/**
 * 
 * Same as A with minor change to colour.
 * 
*/
void partB()
{
    std::ofstream file("barycentric.ppm");
    write_ppm_header(&file);

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            float x = (2 * ((i + 0.5) * invWidth) - 1); 
            float y = (1 - 2 * ((j + 0.5) * invHeight)); 
            Vec3 dir(x, y, lookDirection.z); 
            dir.normalize(); 
            float t, alpha, beta, gamma;
            
            Vec3 fragment(0, 0, 0);
            Vec3 normal(0, 0, 0);
            if (tri.intersect(eye, dir, t, alpha, beta, gamma, normal, fragment))
            {
                // Calculate barycentric interpolation as in assignment 3 
                double r = alpha * c[0].x + beta * c[1].x + gamma * c[2].x;
                double g = alpha * c[0].y + beta * c[1].y + gamma * c[2].y;
                double b = alpha * c[0].z + beta * c[1].z + gamma * c[2].z;
                
                file << (int)r << " " << (int)g << " " << (int)b << " "; 
            }
            else
            {
                file << 255 << " " << 255 << " " << 192 << " ";
            }
        }
    }
    file.close();
}

/**
 * 
 * Calculate ambient light, simmilar logic to part A/B but calculates ambient only.
 * 
*/ 
void partCAmbient()
{
    std::ofstream file("ambient.ppm");
    write_ppm_header(&file);

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            float x = (2 * ((i + 0.5) * invWidth) - 1); 
            float y = (1 - 2 * ((j + 0.5) * invHeight)); 
            Vec3 dir(x, y, lookDirection.z); 
            dir.normalize(); 
            float t, alpha, beta, gamma;
            Vec3 fragment(0, 0, 0);
            Vec3 normal(0, 0, 0);
            if (tri.intersect(eye, dir, t, alpha, beta, gamma, normal, fragment))
            {
                //Set colour to green for easier debug
                Vec3 colour(0, 255, 0);
 
                Vec3 amb = shader.ambient(light, tri.ambient);
                Vec3 finalColour(amb.x * colour.x, amb.y * colour.y, amb.z * colour.z);

                file << (int)finalColour.x << " " << (int)finalColour.y << " " << (int)finalColour.z << " "; 
            }
            else
            {
                file << 255 << " " << 255 << " " << 192 << " ";
            }
        }
    }
    file.close();
}

/**
 * 
 * Calculate diffuse light, simmilar logic to part A/B but calculates diffuse only.
 * 
*/ 
void partCDiffuse()
{
    std::ofstream file("diffuse.ppm");
    write_ppm_header(&file);

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            float x = (2 * ((i + 0.5) * invWidth) - 1); 
            float y = (1 - 2 * ((j + 0.5) * invHeight)); 
            Vec3 dir(x, y, lookDirection.z); 
            dir.normalize(); 
            float t, alpha, beta, gamma;
            
            Vec3 fragment(0, 0, 0);
            Vec3 normal(0, 0, 0);
            if (tri.intersect(eye, dir, t, alpha, beta, gamma, normal, fragment))
            {
                Vec3 colour(0, 255, 0);

                Vec3 diffuseColour = shader.diffuse(light, fragment, normal, tri.diffuse);
                Vec3 finalColour(colour.x * diffuseColour.x , colour.y * diffuseColour.y, colour.z * diffuseColour.z);

                file << (int)finalColour.x << " " << (int)finalColour.y << " " << (int)finalColour.z << " "; 
            }
            else
            {
                file << 255 << " " << 255 << " " << 192 << " ";
            }
        }
    }
    file.close();
}

/**
 * 
 * Calculate specular light, simmilar logic to part A/B but calculates specular only.
 * 
*/ 
void partCSpecular()
{
    std::ofstream file("specular.ppm");
    write_ppm_header(&file);

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            float x = (2 * ((i + 0.5) * invWidth) - 1); 
            float y = (1 - 2 * ((j + 0.5) * invHeight)); 
            Vec3 dir(x, y, lookDirection.z); 
            dir.normalize(); 
            float t, alpha, beta, gamma;
            double diff = 0.0;
            
            
            Vec3 fragment(0, 0, 0);
            Vec3 normal(0, 0, 0);
            if (tri.intersect(eye, dir, t, alpha, beta, gamma, normal, fragment))
            {
                
                Vec3 colour(0, 255, 0);

                Vec3 specularColour = shader.specular(light, fragment, eye, normal, tri.specular, tri.specular_coeff);
                Vec3 finalColour(specularColour.x*255, specularColour.y*255, specularColour.z*255);

                file << (int)finalColour.x << " " << (int)finalColour.y << " " << (int)finalColour.z << " "; 
            }
            else
            {
                file << 255 << " " << 255 << " " << 192 << " ";
            }
        }
    }
    file.close();
}

/**
 * 
 * Calculate blinn-phong lightning, simmilar logic to part A/B but has complete lightning.
 * 
*/ 
void partCPhong()
{
    std::ofstream file("phong.ppm");
    write_ppm_header(&file);

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            float x = (2 * ((i + 0.5) * invWidth) - 1); 
            float y = (1 - 2 * ((j + 0.5) * invHeight)); 
            Vec3 dir(x, y, lookDirection.z); 
            dir.normalize(); 
            float t, alpha, beta, gamma;
            
            Vec3 fragment(0, 0, 0);
            Vec3 normal(0, 0, 0);
            if (tri.intersect(eye, dir, t, alpha, beta, gamma, normal, fragment))
            {

                Vec3 colour(0, 1, 0);

                Vec3 shaderColour = shader.calculatePhong(light, fragment, lookDirection, normal, tri.ambient, tri.diffuse, tri.specular, tri.specular_coeff);

                Vec3 finalColour(colour.x * shaderColour.x, colour.y * shaderColour.y, colour.z * shaderColour.z);

                file << (int)(finalColour.x*255) << " " << (int)(finalColour.y*255) << " " << (int)(finalColour.z*255) << " "; 
            }
            else
            {
                file << 255 << " " << 255 << " " << 192 << " ";
            }
        }
    }
    file.close();
}

int main()
{
    partA();
    partB();
    partCAmbient();
    partCDiffuse();
    partCSpecular();
    partCPhong();
    return 0;
}