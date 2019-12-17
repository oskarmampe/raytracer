#include <iostream>
#include <fstream>
#include <cmath>

float kEpsilon = 0.00000001;

class Vec3
{
public:
    float x, y, z;

    Vec3(const float x0 = 0, const float y0 = 0, const float z0 = 0) : x(x0), y(y0), z(z0) {}

    Vec3 operator-(const Vec3 &v) const
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 operator+(const Vec3 &v) const
    {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }

    Vec3 operator*(const float &r)
    {
        return Vec3(x * r, y * r, z * r);
    }

    Vec3 operator*(const Vec3 &v) const
    {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }

    Vec3 &operator*=(const float &r)
    {
        x *= r, y *= r, z *= r;
        return *this;
    }

    friend Vec3 operator*(const float &r, const Vec3 &v)
    {
        return Vec3(v.x * r, v.y * r, v.z * r);
    }

    void normalize()
    {
        float dist = x * x + y * y + z * z;
        if (dist > 0)
        {
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

    Triangle(const Vec3 p[3], const Vec3 sc[3], const float &refl = 0, const float &transp = 0) : reflection(refl), transparency(transp)
    {
        v0 = Vec3(p[0].x, p[0].y, p[0].z);
        v1 = Vec3(p[1].x, p[1].y, p[1].z);
        v2 = Vec3(p[2].x, p[2].y, p[2].z);
    }

    bool intersect(const Vec3 &orig, const Vec3 &dir,
                   float &t, float &u, float &v, const Vec3 &zxc)
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
        return false; // they are parallel so they don't intersect ! 

    // compute d parameter using equation 2
    float d = N.dotProduct(v0);
    
    // compute t (equation 3)
    t = (N.dotProduct(orig) + d) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind
 
    // compute the intersection point using equation 1
    Vec3 P = orig + t * dir;

    if (zxc.x == 61 && zxc.y == 10)
    {
        std::cout << "P Vector: " << P.x << "< " << P.y << ", " << P.z << std::endl; 
    }
    
 
    // Step 2: inside-outside test
    Vec3 C; // vector perpendicular to triangle's plane
 
    // edge 0
    Vec3 edge0 = v1 - v0; 
    Vec3 vp0 = P - v0;
    C = edge0.crossProduct(vp0);
    if (N.dotProduct(C) < 0) return false; // P is on the right side
 
    // edge 1
    Vec3 edge1 = v2 - v1; 
    Vec3 vp1 = P - v1;
    C = edge1.crossProduct(vp1);
    if ((u = N.dotProduct(C)) < 0)  return false; // P is on the right side
 
    // edge 2
    Vec3 edge2 = v0 - v2; 
    Vec3 vp2 = P - v2;
    C = edge2.crossProduct(vp2);
    if ((v = N.dotProduct(C)) < 0) return false; // P is on the right side;

    u /= denom;
    v /= denom;

    return true; // this ray hits the triangle
    }
};

int main()
{
    int width = 128, height = 128;
    double invWidth = 1.0/128.0, invHeight = 1.0/128.0;
    double fov = 90.0; 
    double scale = tan((fov * 0.5)*M_PI/180.0f);
    double aspect = width/height;

    Vec3 vertices[3] = {Vec3(-0.04688, -0.84375, 1), Vec3(0.5625, 0.5625, 1), Vec3(-0.60938, 0.40625, 1)};
    //Vec3 vertices[3] = {Vec3(61, 10, 1), Vec3(100, 100, 1), Vec3(25, 90, 1)};
    Vec3 c[3] = {Vec3(255, 0, 0), Vec3(0, 255, 0), Vec3(0, 0, 255)};
    Triangle tri = Triangle(vertices, c);
    Vec3 eye(0, 0, 0);
    Vec3 lookDirection(0, 0, 1);
    Vec3 upVector(0, 1, 0);

    std::ofstream file("output.ppm");

    file << "P3" << '\n';
    file << "#Oskar Mampe" << '\n';
    file << "128 128" << '\n';
    file << "255" << '\n';

    std::cout << "scale: " << scale << std::endl;
    std::cout << "aspect: " << aspect << std::endl;
    double topEdge = lookDirection.z * scale;
	double leftEdge = -topEdge * aspect;

    std::cout << "topEdge: " << topEdge << std::endl;
    std::cout << "leftEdge: " << leftEdge << std::endl;


	double step_x = 2.0*(-leftEdge) / static_cast<float>(width);
	double step_y = 2.0*topEdge / static_cast<double>(height);

    std::cout << "invHeight: " << invHeight << std::endl;
    std::cout << "invWidth: " << invWidth << std::endl;



    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {

            //double x = leftEdge + step_x / 2.0 + static_cast<int>(i) * step_x;
			//double y = topEdge - step_y / 2.0 - static_cast<int>(j) * step_y;
            /*
            for each pixel p_ij = (i,j)
            let R_ij be the ray from E through p_ij
            cast ray R into scene
            find point P where R first intersects an object
            compute lighting at point P
            store in image[i,j]
            */
            float x = (2 * ((i + 0.5) * invWidth) - 1); 
            float y = (1 - 2 * ((j + 0.5) * invHeight)); 
            Vec3 dir(x, y, lookDirection.z); 
            dir.normalize(); 
            float t, u, v;

            if (i == 61 && j == 10)
            {
                std::cout << "61, 10: " << std::endl;
                std::cout << x << ", " << y << std::endl;
                std::cout << dir.x << ", " << dir.y << std::endl;
            }

            if (i == 100 && j == 100)
            {
                std::cout << "100, 100: " << std::endl;
                std::cout << x << ", " << y << std::endl;
                std::cout << dir.x << ", " << dir.y << std::endl;
            }

            if (i == 25 && j == 90)
            {
                std::cout << "25, 90: " << std::endl;
                std::cout << x << ", " << y << std::endl;
                std::cout << dir.x << ", " << dir.y << std::endl;
            }
            
            Vec3 orig(i, j, 1);
            if (tri.intersect(eye, dir, t, u, v, orig))
            {
                //file << 0 << " " << 0 << " " << 0 << " ";
                double alpha = u;
                double beta = v;
                double gamma = (1 - u - v);
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