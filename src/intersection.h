#ifndef INTERSECTION_H
#define INTERSECTION_H
#include "ray.h"
#include "sphere.h"
#include "kdtree.h"
#include "aabb.h"
#include <algorithm>

namespace Intersections
{
    struct Intersection
    {
        enum Type
        {
            AABB,
            Mesh,
            Sphere,
            Triangle,
        };

        Type  type;
        float t, u, v;
        bool  backface;
        glm::vec3 point;
        glm::vec3 normal;
        idx_vec3  triangle;
        float tmin, tmax;
    };

    const float EPSILON = 1e-06;

    bool RayTri(const Ray &ray, const idx_vec3 indices, const glm::vec3 vertices[], Intersection &info);
    bool RaySphere(const Ray &ray, const Sphere &sphere, Intersection &info);
    bool RayAABB(const Ray &ray, const AABB &aabb, Intersection &info);
    bool RayKDTree(const Ray &ray, const KDTree &tree, Intersection &info);
    bool RayKDNode(const Ray &ray, const KDNode &node, const AABB &bounds, const std::vector<glm::vec3> vertices, Intersection &info);
};

#endif
