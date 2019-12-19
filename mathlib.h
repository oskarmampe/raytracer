#include <cmath>
#include <algorithm>

float kEpsilon = 0.00000001;
/**
 * 
 * Simple implementation of a Vector. Only floating point is allowed.
 *
*/
class Vec3
{
public:
    double x, y, z;

    Vec3(const double x0 = 0, const double y0 = 0, const double z0 = 0) : x(x0), y(y0), z(z0)
    {
        // Required constructor
    }

    // Overloading operators to work as intended.
    Vec3 operator-(const Vec3 &v) const
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 operator+(const Vec3 &v) const
    {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }

    Vec3 operator*(const double &r)
    {
        return Vec3(x * r, y * r, z * r);
    }

    Vec3 operator*(const Vec3 &v) const
    {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }

    friend Vec3 operator*(const double &r, const Vec3 &v)
    {
        return Vec3(v.x * r, v.y * r, v.z * r);
    }

    // Normalizing the vector. This normalizes the current vector and does not create a copy.
    void normalize()
    {
        float dist = x * x + y * y + z * z;
        if (dist > 0)
        {
            float factor = 1 / sqrt(dist);
            x *= factor, y *= factor, z *= factor;
        }
    }
    
    double dotProduct(const Vec3 &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3 crossProduct(const Vec3 &v) const
    {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

// Compute reflection direction
Vec3 reflect(const Vec3 &I, const Vec3 &N)
{
    float dp = I.dotProduct(N);
    return I - 2 * dp * N;
}