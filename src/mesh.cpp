#include "mesh.h"
#include "intersection.h"
#include "ply.h"
#include "scene.h"
#include "SDL.h"

PlyMesh::PlyMesh(std::string filename, bool center)
	: has_normals(false)
{
	PLY ply(filename);
	auto &vertex_element = ply.elements["vertex"];
	float x, y, z;
	if(!vertex_element.has_property("x") &&
	   !vertex_element.has_property("y") &&
	   !vertex_element.has_property("z"))
	{
		throw std::exception("ply file must contain 3 vertex componenents");
	}
	vertices.resize(vertex_element.count);
	for(uint32_t i = 0; i < vertex_element.count; i++)
	{
		x = vertex_element.get_property("x", i).f;
		y = vertex_element.get_property("y", i).f;
		z = vertex_element.get_property("z", i).f;
		vertices[i] = glm::vec3(x, y, z);
	}
	if(vertex_element.has_property("nx") &&
	   vertex_element.has_property("ny") &&
	   vertex_element.has_property("nz"))
	{
		has_normals = true;
		for(uint32_t i = 0; i < vertex_element.count; i++)
		{
			x = vertex_element.get_property("nx", i).f;
			y = vertex_element.get_property("ny", i).f;
			z = vertex_element.get_property("nz", i).f;
			normals.push_back(glm::vec3(x, y, z));
		}
	}
	auto &face_element = ply.elements["face"];
	idx_vec3 face;
	faces.resize(face_element.count);
	for(uint32_t i =0 ; i < face_element.count; i++)
	{
		if(face_element.list_data[i].size() == 3)
		{
			face[0] = face_element.list_data[i][0].i;
			face[1] = face_element.list_data[i][1].i;
			face[2] = face_element.list_data[i][2].i;
			faces.push_back(face);
		}
		else if(face_element.list_data[i].size() == 4)
		{
			face[0] = face_element.list_data[i][0].i;
			face[1] = face_element.list_data[i][1].i;
			face[2] = face_element.list_data[i][2].i;
			faces.push_back(face);
			face[0] = face_element.list_data[i][2].i;
			face[1] = face_element.list_data[i][3].i;
			face[2] = face_element.list_data[i][0].i;
			faces.push_back(face);
		}
		else
		{
			throw std::exception("only triangles or quads are supported.");
		}
	}
	if(center)
	{
		glm::vec3 mean(0.0);
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
					info.object   = this;
                    retval        = true;
                    info          = temp_info;
                    info.triangle = face;
                }
            }
        }
    }
    return retval;
}


Colour PlyMesh::Shade(const Ray &ray, const Intersections::Record &info, Scene &scene)
{
	glm::vec3 normal(1.0);
	if(has_normals)
	{
		auto v0 = normals[info.triangle[0]] * (1 -info.u - info.v);
		auto v1 = normals[info.triangle[1]] * info.u;
		auto v2 = normals[info.triangle[2]] * info.v;
		normal = v0 + v1 + v2;
		normal += 1.0f;
		normal /= 2.0f;
		if(shadow)
		{
			glm::vec3 point = ray.point_at(info.t);
			for(const auto *light : scene.lights)
			{
				//calculate direction to light.
				if(const PointLight *plight = dynamic_cast<const PointLight*>(light))
				{
					Ray new_ray = Ray(point, glm::normalize(plight->origin - point));
					Intersections::Record shadow_record;
					if(scene.Intersects(new_ray, shadow_record))
					{
						return Colour(0, 0, 0);
					}
				}
			}

		}
	}
	return normal;
}

KDMesh::KDMesh(std::string filename, uint32_t max_depth, uint32_t min_elems, bool center)
: PlyMesh(filename, center)
, node(NULL, vertices, faces, aabb, max_depth, min_elems, 0)
{}

bool KDMesh::Intersects(const Ray &ray, Intersections::Record &info)
{
    if(Intersections::RayKDNode(ray, node, aabb, vertices, info))
	{
		info.type = Intersections::Record::Mesh;
		info.object = this;
		return true;
	}
	return false;
}