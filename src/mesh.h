#ifndef MESH_H
#define MESH_H
#include <vector>
#include <fstream>
#include <exception>
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "object.h"
#include "aabb.h"
#include "types.h"
#include "kdtree.h"

class Mesh : public Object
{
public:
};

class PlyMesh: public Mesh
{
public:
    PlyMesh(std::string filename)
    {
        std::ifstream input(filename);
        std::string temp;
        input >> temp;
        if(temp != "ply") throw std::exception();
        uint32_t index_count  = 0;
        uint32_t vertex_count = 0;
        while(temp != "end_header")
        {
            input >> temp;
            if(temp == "element")
            {
                input >> temp;
                if(temp == "vertex")
                {
                    input >> vertex_count;
                }
                else if(temp == "face")
                {
                    input >> index_count;
                }
            }
        }

        for(auto i = 0u ; i< vertex_count; i++)
        {
            float temp[3];
            for(auto i = 0; i < 3; i++)
                input >> temp[i];
            vertices.push_back(glm::vec3(temp[0], temp[1], temp[2]));
            std::string t;
            std::getline(input, t);
        }
        for(auto i = 0u; i < index_count; i++)
        {
            uint32_t count;
            uint32_t temp[4];
            input >> count;
            for(auto i = 0u ; i < count; i++)
            {
                input >> temp[i];
            }
            faces.push_back(idx_vec3(temp[0], temp[1], temp[2]));
            if(count == 4)
            {
                faces.push_back(idx_vec3(temp[2], temp[3], temp[0]));
            }
            std::string t;
            std::getline(input, t);
        }
        aabb = AABB(vertices);
    }

    bool Intersects(const Ray &ray, Intersections::Intersection &info)
    {
        bool retval = false;
        if(aabb.Intersects(ray, info))
        {
            info.t = std::numeric_limits<float>().max();
            for(auto face : faces)
            {
                Intersections::Intersection temp_info;
                if(Intersections::RayTri(ray, face, vertices.data(), temp_info))
                {
                    if(temp_info.t < info.t)
                    {
                        retval = true;
                        info = temp_info;
                        info.triangle = face;
                    }
                }
            }
        }
        return retval;
    }

    std::vector<idx_vec3> faces;
    std::vector<glm::vec3>  vertices;
    AABB aabb;
};

class KDMesh : public PlyMesh
{
public:
    KDMesh(std::string filename, uint32_t max_depth = 5, uint32_t min_elems = 3)
    : PlyMesh(filename)
    , node(vertices, faces, aabb, max_depth, min_elems, 0)
    {}

    bool Intersects(const Ray &ray, Intersections::Intersection &info)
    {
        return Intersections::RayKDNode(ray, node, aabb, vertices, info);
    }

    KDNode node;
};

#endif
