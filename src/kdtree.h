#ifndef KDTREE_H
#define KDTREE_H
#include <stdint.h>
#include <string.h>
#include "aabb.h"
#include "types.h"

class KDNode
{
public:
    KDNode(const std::vector<glm::vec3>  &vertices,
           const std::vector<idx_vec3> &faces,
           const AABB &bounds, uint32_t depth, uint32_t min_elems, uint8_t axis)
    : leaf(false)
    , split(0.0f)
    , axis(axis)
    {
        memset(children, 0x00, sizeof(children));
        if(depth == 0 || faces.size() < min_elems)
        {
            leaf = true;
            this->faces = faces;
        }
        else
        {
            //Calculate the split
            uint32_t count = 0;
            for(auto v : vertices)
            {
                if(bounds.min[axis] <= v[axis] && v[axis] < bounds.max[axis])
                {
                    count += 1;
                    split += v[axis];
                }
            }
            split /= count;

            //Seperate the triangles
            std::vector<idx_vec3> child_faces[2];
            AABB                  child_bounds[2] = {bounds, bounds};

            child_bounds[0].max[axis] = split;
            child_bounds[1].min[axis] = split;
            uint32_t in_count = 0;
            for(auto &f : faces)
            {
                bool  temp = false;
                if(((bounds.min[axis] <= vertices[f[0]][axis]) && (vertices[f[0]][axis] < split)) ||
                   ((bounds.min[axis] <= vertices[f[1]][axis]) && (vertices[f[1]][axis] < split)) ||
                   ((bounds.min[axis] <= vertices[f[2]][axis]) && (vertices[f[2]][axis] < split)))
                {
                    child_faces[0].push_back(f);
                    temp = true;
                }
                if(((split <= vertices[f[0]][axis]) && (vertices[f[0]][axis] < bounds.max[axis])) ||
                   ((split <= vertices[f[1]][axis]) && (vertices[f[1]][axis] < bounds.max[axis])) ||
                   ((split <= vertices[f[2]][axis]) && (vertices[f[2]][axis] < bounds.max[axis])))
                {
                    child_faces[1].push_back(f);
                    temp = true;
                }
                if(temp) in_count += 1;
            }
#if 0
            if(in_count != faces.size())
            {
                std::cerr << "Geometry lost" << std::endl;
                exit(0);
            }
#endif
            if(child_faces[0].size() && child_faces[1].size())
            {
                for(auto i = 0; i < 2; i++)
                {
                    children[i] = new KDNode(vertices, child_faces[i], child_bounds[i], depth - 1, min_elems, (axis + 1) % 3);
                }
            }
            else
            {
                leaf = true;
                this->faces = faces;
            }
        }
    }

    KDNode *find(const idx_vec3 &face)
    {
        if(leaf)
        {
            for(auto f : faces)
            {
                if(face == f)
                {
                    return this;
                }
            }
            return NULL;
        }
        else
        {
            KDNode *t = children[0]->find(face);
            if(t) return t;
            t = children[1]->find(face);
            return t;
        }
    }

    bool               leaf;
    float              split;
    uint8_t            axis;
    std::vector<idx_vec3> faces;
    KDNode *children[2];
};

class KDTree
{
public:
    KDTree(const std::vector<idx_vec3> &faces,const std::vector<glm::vec3> &vertices, uint32_t max_depth = 5, uint32_t min_elems = 3)
    : vertices(vertices)
    , faces(faces)
    , bounds(vertices)
    , node(vertices, faces, bounds, max_depth, min_elems, 0)
    {}

    std::vector<glm::vec3> vertices;
    std::vector<idx_vec3>  faces;
    AABB    bounds;
    KDNode  node;
};


#endif
