#ifndef MESH_H
#define MESH_H
#include <vector>
#include <fstream>
#include <exception>
#include <string>
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
    PlyMesh(std::string filename, bool center = false);

    bool Intersects(const Ray &ray, Intersections::Record &info);

    std::vector<idx_vec3>  faces;
    std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	bool has_normals;
    AABB aabb;
};

class KDMesh : public PlyMesh
{
public:
    KDMesh(std::string filename, uint32_t max_depth = 10, uint32_t min_elems = 3, bool center = false);

    bool Intersects(const Ray &ray, Intersections::Record &info);

    KDNode node;
};

#endif
