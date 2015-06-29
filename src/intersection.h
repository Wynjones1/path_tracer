#ifndef INTERSECTION_H
#define INTERSECTION_H
#include "ray.h"
#include "sphere.h"
#include "kdtree.h"
#include "aabb.h"
#include "mesh.h"
#include <algorithm>

namespace Intersections
{
    struct Record
    {
        enum Type
        {
			None,
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
		PlyMesh   *mesh;
        float tmin, tmax;

		Record()
		: type(None)
		, t(std::numeric_limits<float>::max())
		, u(0.0f)
		, v(0.0f)
		, backface(false)
		, point(0.0f)
		, normal(0.0f)
		, triangle(0)
		, tmin(std::numeric_limits<float>::max())
		, tmax(-std::numeric_limits<float>::max())
		{}
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
