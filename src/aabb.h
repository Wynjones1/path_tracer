#ifndef AABB_H
#define AABB_H
#include "object.h"
#include <vector>
#include <iostream>

class AABB : public Object
{
public:
    AABB(const glm::vec3 &min, const glm::vec3 &max);
    AABB(const std::vector<glm::vec3> &vertices); 
    AABB();

    bool Intersects(const Ray &ray, Intersections::Record &info);
    float volume() const
    {
        return abs(max[0] - min[0]) *
               abs(max[1] - min[1]) *
               abs(max[2] - min[2]);
                
    }

	bool contains(const glm::vec3 &point) const
	{
		return min.x   <= point.x && min.y   <= point.y && min.z   <= point.z &&
			   point.x <  max.x   && point.y <  max.y   && point.z <  max.z;
	}

    glm::vec3 min;
    glm::vec3 max;
};

std::ostream &operator <<(std::ostream &stream, const AABB &aabb);
#endif
