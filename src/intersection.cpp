#include "intersection.h"
#include "gtc/type_ptr.hpp"

namespace Intersections
{
    bool RayTri(const Ray &ray, const idx_vec3 indices, const glm::vec3 vertices[], Intersection &info)
    {
        info.type = Intersection::Type::Triangle;
        const glm::vec3 &v0 = vertices[indices[0]];
        const glm::vec3 &v1 = vertices[indices[1]];
        const glm::vec3 &v2 = vertices[indices[2]];
        glm::vec3 e1  = v1 - v0;
        glm::vec3 e2  = v2 - v0;
        glm::vec3 t   = ray.origin - v0;
        glm::vec3 p   = glm::cross(ray.direction, e2);
        glm::vec3 q   = glm::cross(t, e1);

        float det = glm::dot(p, e1);
        if(det > -EPSILON && det < EPSILON)
        {
            return false;
        }

        float inv_det = 1.0f / det;
        info.t = inv_det * glm::dot(q, e2);

        if(info.t < EPSILON)
        {
            return false;
        }

        info.u = glm::dot(p, t)             * inv_det;
        info.v = glm::dot(q, ray.direction) * inv_det;

        if(info.u < -EPSILON || info.u > (1.0f + EPSILON))
        {
            return false;
        }
        if(info.v < -EPSILON || info.v > (1.0f + EPSILON))
        {
            return false;
        }
        if(info.u + info.v > (1.0f + EPSILON))
        {
            return false;
        }

        info.backface = inv_det < 0.0f;
        return true;
    }

    bool RaySphere(const Ray &ray, const Sphere &sphere, Intersection &info)
    {
        info.type = Intersection::Type::Sphere;
        float a = glm::dot(ray.direction, ray.direction);
        glm::vec3 origin = ray.origin - sphere.origin;
        float b = 2 * glm::dot(origin, ray.direction);
        float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;
        float disc = b * b - 4 * a * c;

        if(disc < 0.0)
        {
            return false;
        }

        float sq = sqrt(disc);
        float t0 = (- b - sq) / 2;
        float t1 = (- b + sq) / 2;

        if(t0 > t1)
        {
            std::swap(t0, t1);
        }

        if(t1 < EPSILON)
        {
            info.t = t1;
        }
        else
        {
            info.t = t0;
        }
        info.point  = ray.direction * info.t + ray.origin;
        info.normal = glm::normalize((info.point - sphere.origin) / sphere.radius);
        return true;
    }

    bool RayAABB(const Ray &ray, const AABB &bounds, Intersection &info)
    {
        info.tmin = std::numeric_limits<float>().min();
        info.tmax = std::numeric_limits<float>().max();
        info.type = Intersections::Intersection::AABB;
        for(auto i = 0; i < 3; i++)
        {
            auto t1 = (bounds.min[i]  - ray.origin[i]) / ray.direction[i];
            auto t2 = (bounds.max[i]  - ray.origin[i]) / ray.direction[i];
            if(t1 > t2)
            {
                std::swap(t1, t2);
            }
            if(t1 > info.tmin)
            {
                info.tmin = t1;
            }
            if(t2 < info.tmax)
            {
                info.tmax = t2;
            }
            if(info.tmin > info.tmax || info.tmax < EPSILON)
            {
                return false;
            }
        }
        return true;
    }

    static inline 
    bool RayKDNodeLeaf( const Ray    &ray,
                    const KDNode &node,
                    const AABB   &bounds,
                    const std::vector<glm::vec3> vertices,
                    Intersection &info)
    {
        Intersection tri_info;
        info.t = std::numeric_limits<float>().max();
        bool retval = false;
        for(auto &f : node.faces)
        {
            if(Intersections::RayTri(ray, f, vertices.data(), tri_info))
            {
                if(tri_info.t < info.t)
                {
                    glm::vec3 point = ray.origin + ray.direction * tri_info.t;
                    if(((bounds.min[0] - EPSILON) >= point[0]) ||
                       ((bounds.min[1] - EPSILON) >= point[1]) ||
                       ((bounds.min[2] - EPSILON) >= point[2]) ||
                       ((bounds.max[0] + EPSILON) <  point[0]) ||
                       ((bounds.max[1] + EPSILON) <  point[1]) ||
                       ((bounds.max[2] + EPSILON) <  point[2]))
                    {
                        continue;
                    }
                    info      = tri_info;
                    info.type = Intersection::Mesh;
                    retval    = true;
                }
            }
        }
        return retval;
    }

    bool RayKDNode( const Ray    &ray,
                    const KDNode &node,
                    const AABB   &bounds,
                    const std::vector<glm::vec3> vertices,
                    Intersection &info)
    {
        if(RayAABB(ray, bounds, info))
        {
            if(node.leaf)
            {
                return RayKDNodeLeaf(ray, node, bounds, vertices, info);
            }
            else
            {
                const auto axis      = node.axis;
                const auto origin    = ray.origin[axis];
                const auto direction = ray.direction[axis];
                const auto tmin      = info.tmin;
                const auto tmax      = info.tmax;
                const auto tsplit    = (node.split - origin) / direction;
                const bool on_left   = origin < node.split;

                const KDNode &near = *node.children[on_left ? 0 : 1];
                const KDNode &far  = *node.children[on_left ? 1 : 0];
                AABB near_bounds   = bounds;
                AABB far_bounds    = bounds;
                if(on_left)
                {
                    near_bounds.max[axis] = tsplit;
                    far_bounds.min[axis]  = tsplit;
                }
                else
                {
                    near_bounds.min[axis] = tsplit;
                    far_bounds.max[axis]  = tsplit;
                }
#if 1
                if( RayKDNode(ray, near, near_bounds, vertices, info) ||
                    RayKDNode(ray, far, far_bounds, vertices, info))
                {
                    return true;
                }
#else
#endif
            }
        }
        return false;
    }

    bool RayKDTree(const Ray &ray, const KDTree &tree, Intersection &info)
    {
        //    return RayKDTreeNode(ray, tree.node, tree.bounds, tree.vertices, info);
        if(RayAABB(ray, tree.bounds, info))
        {
            for(auto &face : tree.faces)
            {
                if(RayTri(ray, face, tree.vertices.data(), info))
                {
                    return true;
                }
            }
        }
        return false;
    }
};


