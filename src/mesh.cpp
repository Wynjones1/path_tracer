#include "mesh.h"
#include "intersection.h"
#include "ply.h"

PlyMesh::PlyMesh(std::string filename, bool center)
	: has_normals(false)
{
	PLY ply(filename);
	auto element = ply.elements["vertex"];
	float x, y, z;
	if(!element.has_property("x") &&
	   !element.has_property("y") &&
	   !element.has_property("z"))
	{
		throw std::exception("ply file must contain 3 vertex componenents");
	}
	for(uint32_t i = 0; i < element.count; i++)
	{
		x = std::stof(element.get_property("x", i));
		y = std::stof(element.get_property("y", i));
		z = std::stof(element.get_property("z", i));
		vertices.push_back(glm::vec3(x, y, z));
	}
	if(element.has_property("nx") &&
	   element.has_property("ny") &&
	   element.has_property("nz"))
	{
		has_normals = true;
		for(uint32_t i = 0; i < element.count; i++)
		{
			x = std::stof(element.get_property("nx", i));
			y = std::stof(element.get_property("ny", i));
			z = std::stof(element.get_property("nz", i));
			normals.push_back(glm::vec3(x, y, z));
		}
	}
	element = ply.elements["face"];
	idx_vec3 face;
	for(uint32_t i =0 ; i < element.count; i++)
	{
		if(element.list_data[i].size() == 3)
		{
			face[0] = std::stoi(element.list_data[i][0]);
			face[1] = std::stoi(element.list_data[i][1]);
			face[2] = std::stoi(element.list_data[i][2]);
			faces.push_back(face);
		}
		else if(element.list_data[i].size() == 4)
		{
			face[0] = std::stoi(element.list_data[i][0]);
			face[1] = std::stoi(element.list_data[i][1]);
			face[2] = std::stoi(element.list_data[i][2]);
			faces.push_back(face);
			face[0] = std::stoi(element.list_data[i][2]);
			face[1] = std::stoi(element.list_data[i][3]);
			face[2] = std::stoi(element.list_data[i][0]);
			faces.push_back(face);
		}
		else
		{
			throw std::exception("only triangles or quads are supported.");
		}
	}
	glm::vec3 mean(0.0);
	if(center)
	{
		for(const auto &v : vertices)
		{
			mean += v;
		}
		mean /= vertices.size();
		for(auto &v : vertices)
		{
			v -= mean;
		}
	}
    aabb = AABB(vertices);
}

bool PlyMesh::Intersects(const Ray &ray, Intersections::Record &info)
{
    bool retval = false;
    if(aabb.Intersects(ray, info))
    {
        info.t = std::numeric_limits<float>().max();
        for(auto face : faces)
        {
            Intersections::Record temp_info;
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

KDMesh::KDMesh(std::string filename, uint32_t max_depth, uint32_t min_elems, bool center)
: PlyMesh(filename, center)
, node(NULL, vertices, faces, aabb, max_depth, min_elems, 0)
{}

bool KDMesh::Intersects(const Ray &ray, Intersections::Record &info)
{
    return Intersections::RayKDNode(ray, node, aabb, vertices, info);
}