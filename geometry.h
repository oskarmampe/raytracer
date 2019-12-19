#include <iostream>
#include "mathlib.h"

/**
 * 
 * A struct representing a light, using location and the colour of the light.
 * 
*/
struct LightStruct
{
    Vec3 position;
    Vec3 colour;
};


/**
 * 
 * A class representing a triangle
 * 
*/
class Triangle
{
public:
    Vec3 v0, v1, v2, surfaceColour[3];
    double ambient, diffuse, specular;
    double specular_coeff;

    // Constructer for a triangle, including material properties, vertices, and surface colour.
    Triangle(const Vec3 p[3], const Vec3 sc[3], const double amb, const double dif, const double spec, const double coeff) : specular_coeff(coeff), ambient(amb), diffuse(dif), specular(spec)
    {
        v0 = Vec3(p[0].x, p[0].y, p[0].z);
        v1 = Vec3(p[1].x, p[1].y, p[1].z);
        v2 = Vec3(p[2].x, p[2].y, p[2].z);
    }

    // Check whether a ray intersects a triangle.
    bool intersect(const Vec3 &eye, const Vec3 &dir,
                   float &t, float &alpha, float &beta, float &gamma, Vec3 &normal, Vec3 &fragment)
    {
    //Compute planar coordinate system
    Vec3 e0 = v1 - v0;
    Vec3 e1 = v2 - v0;
    normal = e0.crossProduct(e1); 
    float normal_area = normal.dotProduct(normal);
    
    // Finding P, first check if parallel
    float normalDotRay = normal.dotProduct(dir);

    // If parallel, or close to, then return false, two parallel lines cannot intersect
    if (fabs(normalDotRay) < kEpsilon) 
    {
        return false; 
    }

    // compute d parameter using equation 2
    float d = normal.dotProduct(v0);
    
    // Find the intersection point
    t = (normal.dotProduct(eye) + d) / normalDotRay;
    if (t < 0)
    {
        return false;
    }
 
    // Therefore, the point has these coordinates
    fragment = eye + t * dir;
 
    // Test whether the triangle is inside using barycentric coordinates and half-plane test

    // First edge
    Vec3 edge0 = v1 - v0; 
    Vec3 fv0 = fragment - v0;
    Vec3 cfv0 = edge0.crossProduct(fv0);

    // Second Edge
    Vec3 edge1 = v2 - v1; 
    Vec3 fv1 = fragment - v1;
    Vec3 cfv1 = edge1.crossProduct(fv1);

    // Third Edge
    Vec3 edge2 = v0 - v2; 
    Vec3 fv2 = fragment - v2;
    Vec3 cfv2 = edge2.crossProduct(fv2);

    // Barycentric coordinates
    gamma = normal.dotProduct(cfv0);
    alpha = normal.dotProduct(cfv1);
    beta = normal.dotProduct(cfv2);

    if (gamma < 0 || alpha < 0 || beta < 0)
    {
        return false;
    }

    alpha /= normal_area;
    beta /= normal_area;
    gamma /= normal_area;
    // Check successful, the ray intersects the triangle
    return true; 
    }
};

/**
 * 
 * Shader using phong technique
 * 
*/
class PhongShader
{
    public:
        PhongShader()
        {
            //Required empty constructor
        }

        // Calculate ambient light using ambient material coefficient and the colour of the light
        Vec3 ambient(LightStruct light, double ambient_weight)
        {
            Vec3 result(ambient_weight * light.colour.x, ambient_weight * light.colour.y, ambient_weight * light.colour.z);
            return result;
        }

        // Calculate diffuse light using material coefficient, position of the fragment, normal and the colour/position of the light
        Vec3 diffuse(LightStruct light, Vec3 fragmentPos, Vec3 objectNormal, double diffuse_weight)
        {
            objectNormal.normalize();// Normalize just in case

            // Get the direction of the light
            Vec3 lightDir = fragmentPos - light.position; 
            lightDir.normalize();

            // Calculate diffuse
            double coeff = objectNormal.dotProduct(lightDir);
            double diff = std::max(coeff, 0.0);
            Vec3 result(light.colour.x * diff * diffuse_weight, light.colour.y * diff * diffuse_weight, light.colour.z * diff * diffuse_weight);
            return result;
        }

        // Calculate specular light using material coefficients, position of the fragment, eye position, normal and the colour/position of the light
        Vec3 specular(LightStruct light, Vec3 fragmentPos, Vec3 viewDirection, Vec3 objectNormal, double specular_weight, double specular_coeff)
        {
            objectNormal.normalize();// Normalize just in case
            
            // Get the direction of light
            Vec3 lightDir = fragmentPos - light.position; 
            lightDir.normalize();  

            // Get the view direction
            Vec3 viewDir(viewDirection.x - fragmentPos.x, viewDirection.y - fragmentPos.y, viewDirection.z - fragmentPos.z);
            viewDir.normalize();

            // Get the reflect vector
            Vec3 reflectDir = reflect(Vec3(lightDir.x, lightDir.y, lightDir.z), objectNormal);  

            // Calculate specular lightning
            float spec = std::pow(std::max(viewDir.dotProduct(reflectDir), 0.0), specular_coeff);
            Vec3 result = Vec3(light.colour.x * spec * specular_weight, light.colour.y * spec * specular_weight, light.colour.z * spec * specular_weight); 
            return result;
        }

        // Calculate all three light and return the result
        Vec3 calculatePhong(LightStruct light, Vec3 fragmentPos, Vec3 viewDirection, Vec3 objectNormal, double ambient_weight, double diffuse_weight, double specular_weight, double specular_coeff)
        {
            Vec3 amb = ambient(light, ambient_weight);
            Vec3 diff = diffuse(light, fragmentPos, objectNormal, diffuse_weight);
            Vec3 spec = specular(light, fragmentPos, viewDirection, objectNormal, specular_weight, specular_coeff);

            return Vec3(amb.x + diff.x + spec.x, amb.y + diff.y + spec.y, amb.z + diff.z + spec.z);
        }
};
