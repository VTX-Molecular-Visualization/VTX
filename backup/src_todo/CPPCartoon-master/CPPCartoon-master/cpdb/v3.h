#define v3_HPP
 
#include <iostream>
 
class v3
{
    private:
 
    public:
     
        // A v3 simply has three properties called x, y and z
        float x, y, z;
        // ------------ Constructors ------------

 
        // Default constructor
        v3() { x = y = z = 0; };
 
        // Three parameter constructor
        v3(float xValue, float yValue, float zValue)
        {
            x = xValue;
            y = yValue;
            z = zValue;
        }
 
        // ------------ Getters and setters ------------
 
        void set(const float &xValue, const float &yValue, const float &zValue)
        {
            x = xValue;
            y = yValue;
            z = zValue;
        }
 
        float getX() const { return x; }
        float getY() const { return y; }
        float getZ() const { return z; }
 
        void setX(const float &xValue) { x = xValue; }
        void setY(const float &yValue) { y = yValue; }
        void setZ(const float &zValue) { z = zValue; }
 
        // ------------ Helper methods ------------
 
        // Method to reset a vector to zero
        void zero()
        {
            x = y = z = 0;
        }
 
        // Method to normalise a vector
        void normalize()
        {
            // Calculate the magnitude of our vector
            float magnitude = sqrtf((x * x) + (y * y) + (z * z));
 
            // As long as the magnitude isn't zero, divide each element by the magnitude
            // to get the normalised value between -1 and +1
            if (magnitude != 0)
            {
                x /= magnitude;
                y /= magnitude;
                z /= magnitude;
            }
        }
        v3 normalized()
        {
            v3 res = v3(x,y,z);
            // Calculate the magnitude of our vector
            float magnitude = sqrtf((res.x * res.x) + (res.y * res.y) + (res.z * res.z));
 
            // As long as the magnitude isn't zero, divide each element by the magnitude
            // to get the normalised value between -1 and +1
            if (magnitude != 0)
            {
                res.x /= magnitude;
                res.y /= magnitude;
                res.z /= magnitude;
            }
            return res;
        }
        // Static method to calculate and return the scalar dot product of two vectors
        //
        // Note: The dot product of two vectors tell us things about the angle between
        // the vectors. That is, it tells us if they are pointing in the same direction
        // (i.e. are they parallel? If so, the dot product will be 1), or if they're
        // perpendicular (i.e. at 90 degrees to each other) the dot product will be 0,
        // or if they're pointing in opposite directions then the dot product will be -1.
        //
        // Usage example: double foo = v3<double>::dotProduct(vectorA, vectorB);
        static float dotProduct(const v3 &vec1, const v3 &vec2)
        {
            return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
        }
 
        // Non-static method to calculate and return the scalar dot product of this vector and another vector
        //
        // Usage example: double foo = vectorA.dotProduct(vectorB);
        float dotProduct(const v3 &vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }
 
        // Static method to calculate and return a vector which is the cross product of two vectors
        //
        // Note: The cross product is simply a vector which is perpendicular to the plane formed by
        // the first two vectors. Think of a desk like the one your laptop or keyboard is sitting on.
        // If you put one pencil pointing directly away from you, and then another pencil pointing to the
        // right so they form a "L" shape, the vector perpendicular to the plane made by these two pencils
        // points directly upwards.
        //
        // Whether the vector is perpendicularly pointing "up" or "down" depends on the "handedness" of the
        // coordinate system that you're using.
        //
        // Further reading: http://en.wikipedia.org/wiki/Cross_product
        //
        // Usage example: v3<double> crossVect = v3<double>::crossProduct(vectorA, vectorB);
        static v3 crossProduct(const v3 &vec1, const v3 &vec2)
        {
            return v3(vec1.y * vec2.z - vec1.z * vec2.y, vec1.z * vec2.x - vec1.x * vec2.z, vec1.x * vec2.y - vec1.y * vec2.x);
        }
 
        // Easy adders
        void addX(float value) { x += value; }
        void addY(float value) { y += value; }
        void addZ(float value) { z += value; }
 
        // Method to return the distance between two vectors in 3D space
        //
        // Note: This is accurate, but not especially fast - depending on your needs you might
        // like to use the Manhattan Distance instead: http://en.wikipedia.org/wiki/Taxicab_geometry
        // There's a good discussion of it here: http://stackoverflow.com/questions/3693514/very-fast-3d-distance-check
        // The gist is, to find if we're within a given distance between two vectors you can use:
        //
        // bool within3DManhattanDistance(v3 c1, v3 c2, float distance)
        // {
        //      float dx = abs(c2.x - c1.x);
        //      if (dx > distance) return false; // too far in x direction
        //
        //      float dy = abs(c2.y - c1.y);
        //      if (dy > distance) return false; // too far in y direction
        //
        //      float dz = abs(c2.z - c1.z);
        //      if (dz > distance) return false; // too far in z direction
        //
        //      return true; // we're within the cube
        // }
        //
        // Or to just calculate the straight Manhattan distance you could use:
        //
        // float getManhattanDistance(v3 c1, v3 c2)
        // {
        //      float dx = abs(c2.x - c1.x);
        //      float dy = abs(c2.y - c1.y);
        //      float dz = abs(c2.z - c1.z);
        //      return dx+dy+dz;
        // }
        //
        static float getDistance(const v3 &v1, const v3 &v2)
        {
            float dx = v2.x - v1.x;
            float dy = v2.y - v1.y;
            float dz = v2.z - v1.z;
 
            return sqrtf(dx * dx + dy * dy + dz * dz);
        }
 
        // Method to display the vector so you can easily check the values
        void display()
        {
            std::cout << "X: " << x << "\t Y: " << y << "\t Z: " << z << std::endl;
        }
 
        // ------------ Overloaded operators ------------
 
        // Overloaded addition operator to add v3s together
        v3 operator+(const v3 &vector) const
        {
            return v3(x + vector.x, y + vector.y, z + vector.z);
        }
 
        // Overloaded add and asssign operator to add v3s together
        void operator+=(const v3 &vector)
        {
            x += vector.x;
            y += vector.y;
            z += vector.z;
        }
 
        // Overloaded subtraction operator to subtract a v3 from another v3
        v3 operator-(const v3 &vector) const
        {
            return v3(x - vector.x, y - vector.y, z - vector.z);
        }
 
        // Overloaded subtract and asssign operator to subtract a v3 from another v3
        void operator-=(const v3 &vector)
        {
            x -= vector.x;
            y -= vector.y;
            z -= vector.z;
        }
 
        // Overloaded multiplication operator to multiply two v3s together
        v3 operator*(const v3 &vector) const
        {
            return v3(x * vector.x, y * vector.y, z * vector.z);
        }
 
        // Overloaded multiply operator to multiply a vector by a scalar
        v3 operator*(const float &value) const
        {
            return v3(x * value, y * value, z * value);
        }
 
        // Overloaded multiply and assign operator to multiply a vector by a scalar
        void operator*=(const float &value)
        {
            x *= value;
            y *= value;
            z *= value;
        }
 
        // Overloaded multiply operator to multiply a vector by a scalar
        v3 operator/(const float &value) const
        {
            return v3(x / value, y / value, z / value);
        }
 
        // Overloaded multiply and assign operator to multiply a vector by a scalar
        void operator/=(const float &value)
        {
            x /= value;
            y /= value;
            z /= value;
        }
};
 
