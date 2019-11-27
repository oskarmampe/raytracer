#include <iostream>
#include <fstream>
#include <cmath>

float kEpsilon = 0.00000001;

class Vec3
{
    public:
        float x, y, z;

        Vec3(const float x0 = 0, const float y0 = 0, const float z0 = 0): x(x0), y(y0), z(z0) {}

        Vec3 operator - (const Vec3 &v) const
        { 
            return Vec3(x - v.x, y - v.y, z - v.z); 
        }
        
        Vec3 operator + (const Vec3 &v) const
        { 
            return Vec3(x + v.x, y + v.y, z + v.z); 
        }


        Vec3 operator * (const float &r)
        { 
            return Vec3(x * r, y * r, z * r); 
        }
        
        Vec3 operator * (const Vec3 &v) const
        {
             return Vec3(x * v.x, y * v.y, z * v.z); 
        }

        Vec3& operator *= (const float &r)
        { 
            x *= r, y *= r, z *= r; 
            return *this; 
        }

        friend Vec3 operator * (const float &r, const Vec3 &v)
        { 
            return Vec3(v.x * r, v.y * r, v.z * r); 
        }

        void normalize()
        {
            float dist = x * x + y * y + z * z;
            if (dist > 0) {
                float factor = 1 / sqrt(dist);
                x *= factor, y *= factor, z *= factor;
            }
        }

        float dotProduct(const Vec3 &v) const
        { 
            return x * v.x + y * v.y + z * v.z; 
        } 

        Vec3 crossProduct(const Vec3 &v) const
        { 
            return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
        }
        
};

class Triangle
{
    public:
        Vec3 v0, v1, v2, surfaceColour[3];
        float transparency, reflection;

        Triangle(const Vec3 p[3], const Vec3 sc[3], const float &refl = 0, const float &transp = 0) : 
        reflection(refl), transparency(transp)
        {
            v0 = Vec3(p[0].x, p[0].y, p[0].z);
            v1 = Vec3(p[1].x, p[1].y, p[1].z);
            v2 = Vec3(p[2].x, p[2].y, p[2].z);
        }

        bool intersect(const Vec3 &orig, const Vec3 &dir,
    float &t, float &u, float &v)
        {
            //compute plane's normal
            Vec3 uv = v1 - v0;
            Vec3 vv = v2 - v0;

            uv.normalize();

            Vec3 n = uv.crossProduct(vv);
            Vec3 w = n.crossProduct(uv);

        }
};

int main() 
{
    int width = 128, height = 128;
 
    float fov = 90; //90?
    float imageAspectRatio = (float)width / (float)height;
    float scale = tan((fov * 0.5) * (M_PI/180));
    Vec3 vertices[3] = {Vec3(61, 10, 1), Vec3(100, 100, 1), Vec3(25, 90, 1)};
    Vec3 colours[3] = {Vec3(255, 0, 0), Vec3(0, 255, 0), Vec3(0, 0, 255)};
    Triangle tri = Triangle(vertices, colours);
    Vec3 eye(0, 0, 0);
    Vec3 lookDirection(0, 0, 1);
    Vec3 upVector(0, 1, 0);

    std::ofstream file("output.ppm");

    file << "P3" << '\n';
    file << "#Oskar Mampe" << '\n';
    file << "128 128" << '\n';
    file << "255" << '\n';

    for(int j = 0; j < height; ++j)
    {
        for(int i = 0; i < width; ++i)
        {
            /*
            for each pixel p_ij = (i,j)
            let R_ij be the ray from E through p_ij
            cast ray R into scene
            find point P where R first intersects an object
            compute lighting at point P
            store in image[i,j]

            */
            float t, u, v;

            if (tri.intersect(eye, lookDirection, t, u, v)) 
            {
                file << 0 << " " << 0 << " " << 0 << " ";
            } 
            else 
            {
                file << 255 << " " << 255 << " " << 255 << " ";
            }
        }
    }
    /*
    for each pixel p_ij = (i,j)
	let R_ij be the ray from E through p_ij
	cast ray R into scene
	find point P where R first intersects an object
	compute lighting at point P
	store in image[i,j]     
    */
    file.close();
    return 0;
}