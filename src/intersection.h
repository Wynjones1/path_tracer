#ifndef INTERSECTION_H
#define INTERSECTION_H
#include "ray.h"
#include "sphere.h"
#include "kdtree.h"
#include "aabb.h"
#include <algorithm>

namespace Intersections
{
    struct Record
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

    const float EPSILON = 1e-06f;

    bool RayTri(const Ray &ray, const idx_vec3 indices, const glm::vec3 vertices[], Record &info);
    bool RaySphere(const Ray &ray, const Sphere &sphere, Record &info);
    bool RayAABB(const Ray &ray, const AABB &aabb, Record &info);
    bool RayKDTree(const Ray &ray, const KDTree &tree, Record &info);
    bool RayKDNode(const Ray &ray, const KDNode &node, const AABB &bounds, const std::vector<glm::vec3> &vertices, Record &info);

	extern KDNode *g_trace_node;
};

#endif
