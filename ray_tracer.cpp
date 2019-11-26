#include <iostream>
#include <fstream>
#include <cmath>

float kEpsilon = 1e-8;

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
            //std::copy(&sc, &sc+3, &surfaceColour);
        }

        bool intersect(const Vec3 &orig, const Vec3 &dir,
    float &t, float &u, float &v)
        {
            // compute plane's normal
            Vec3 v0v1 = v1 - v0;
            Vec3 v0v2 = v2 - v0;
            // no need to normalize
            Vec3 N = v0v1.crossProduct(v0v2); // N
            float denom = N.dotProduct(N);
            
            // Step 1: finding P
            
            // check if ray and plane are parallel ?
            float NdotRayDirection = N.dotProduct(dir);
            if (fabs(NdotRayDirection) < kEpsilon) // almost 0
            {
                std::cout << "Parallel " << std::endl;
                return false; // they are parallel so they don't intersect ! 
            }
            // compute d parameter using equation 2
            float d = N.dotProduct(v0);
            
            // compute t (equation 3)
            t = (N.dotProduct(orig) + d) / NdotRayDirection;
            // check if the triangle is in behind the ray
            if (t < 0)
            {
                std::cout << "Triangle behind " << std::endl;
                return false; // the triangle is behind
            } 
        
            // compute the intersection point using equation 1
            Vec3 P = orig + (t * dir);
        
            // Step 2: inside-outside test
            Vec3 C; // vector perpendicular to triangle's plane
        
            // edge 0
            Vec3 edge0 = v1 - v0; 
            Vec3 vp0 = P - v0;
            C = edge0.crossProduct(vp0);
            if (N.dotProduct(C) < 0)
            {
                std::cout << "P is on the right side: edge 0" << std::endl;
                return false; // P is on the right side
            } 
        
            // edge 1
            Vec3 edge1 = v2 - v1; 
            Vec3 vp1 = P - v1;
            C = edge1.crossProduct(vp1);
            if ((u = N.dotProduct(C)) < 0)  
            {
                std::cout << "P is on the right side: edge 1" << std::endl;
                return false; // P is on the right side
            } 
        
            // edge 2
            Vec3 edge2 = v0 - v2; 
            Vec3 vp2 = P - v2;
            C = edge2.crossProduct(vp2);
            if ((v = N.dotProduct(C)) < 0) 
            {
                std::cout << "P is on the right side: edge 2" << std::endl;
                return false; // P is on the right side
            } 

            u /= denom;
            v /= denom;

            return true; // this ray hits the triangle

        }
};

int main() 
{
    int width = 128, height = 128;
    float eye[3] = {0, 0, 0};
    float rayOriginal[3] = {0, 0, 1};
    float rayDirection[3] = {0, 1, 0};
    float fov = 45; //90?
    float imageAspectRatio = (float)width / (float)height;
    float scale = tan((fov * 0.5) * (M_PI/180));
    Vec3 vertices[3] = {Vec3(61, 10, 1), Vec3(100, 100, 1), Vec3(25, 90, 1)};
    Vec3 colours[3] = {Vec3(255, 0, 0), Vec3(0, 255, 0), Vec3(0, 0, 255)};
    Triangle tri = Triangle(vertices, colours);
    Vec3 orig(0, 0, 0);

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
           // compute primary ray
            float x = (2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)height) * scale;
            Vec3 dir(x, y, -1);
            std::cout << dir.x << dir.y << dir.z << std::endl;
            dir.normalize();
            std::cout << dir.x << dir.y << dir.z << std::endl;
            float t, u, v;

            if (tri.intersect(orig, dir, t, u, v)) {
                // [comment]
                // Interpolate colors using the barycentric coordinates
                // [/comment]
                file << 0 << " " << 0 << " " << 0 << " ";
                //u * cols[0] + v * cols[1] + (1 - u - v) * cols[2];
                // uncomment this line if you want to visualize the row barycentric coordinates
                //*pix = Vec3f(u, v, 1 - u - v);
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