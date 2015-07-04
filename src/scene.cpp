#include "scene.h"

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "object.h"
#include "mesh.h"
#include "config.h"
#include "intersection.h"
#include "camera.h"
#include <stdlib.h>
#include <stdio.h>
#include "glm.hpp"
#include "SDL.h"

using namespace rapidjson;
#define SCENE_VERBOSE 1

template<typename T, typename V>
static Object *read_object(const GenericValue<T, V> &value)
{
#if SCENE_VERBOSE
	std::cout << "reading object..." << std::endl;
	uint32_t start_time = SDL_GetTicks();
#endif
	bool center = false;
	Object *out = NULL;
	if(value.HasMember("center"))
	{
		if(value["center"].IsBool())
		{
			center = value["center"].GetBool();
		}
		else
		{
			throw std::exception("scene: center must be boolean value");
		}
	}
	if(value.HasMember("ply"))
	{
		auto &ply_filename = value["ply"];
		if(ply_filename.IsString())
		{
			std::string temp = ply_filename.GetString();
			out = new KDMesh(PLY_DIRECTORY + temp, 20, 5, center);
		}
	}
	if(!out)
	{
		throw std::exception("Could not create object.");
	}
#if SCENE_VERBOSE
	std::cout << "took " << (SDL_GetTicks() - start_time) / 1000.0f << " seconds." << std::endl;
#endif
	return out;
}

template<typename T, typename V>
static glm::vec3 read_vec3(const GenericValue<T, V> &value)
{
	if(value.IsArray() && value.Size() == 3)
	{
		glm::vec3 out;
		for(int i = 0; i < 3; i++)
		{
			if(value[i].IsNumber())
			{
				out[i] = (float) value[i].GetDouble();
			}
			else
			{
				throw std::exception("vec3 elements must all be numbers");
			}
		}
		return out;
	}
	throw std::exception("could not read vec3");
}

template<typename T, typename V>
static float read_float(const GenericValue<T, V> &value)
{
	return (float)value.GetDouble();
}

template<typename T, typename V>
static Camera *read_camera(const GenericValue<T, V> &value)
{
	if(value.HasMember("type"))
	{
		const auto &camera_type = value["type"];
		if(camera_type.IsString())
		{
			//camera default values.
			glm::vec3 origin  = glm::vec3(0, 0, 0);
			glm::vec3 forward = glm::vec3(0, 0, -1);
			glm::vec3 right   = glm::vec3(1, 0, 0);
			//read the common definitions.
			if(value.HasMember("origin") && value["origin"].IsArray())
			{
				origin = read_vec3(value["origin"]);
			}
			if(value.HasMember("forward") && value["forward"].IsArray())
			{
				forward = read_vec3(value["forward"]);
			}
			if(value.HasMember("right") && value["right"].IsArray())
			{
				right = read_vec3(value["right"]);
			}
			if(std::string(camera_type.GetString()) == "ortho")
			{
				//ortho camera defaults
				float width  = 1.0f;
				float height = 1.0f;
				if(value.HasMember("width") && value["width"].IsNumber())
				{
					width = read_float(value["width"]);
				}
				if(value.HasMember("height") && value["height"].IsNumber())
				{
					height = read_float(value["height"]);
				}
				return (Camera*)new OrthoCamera(origin, forward, right, width, height);
			}
			if(std::string(camera_type.GetString()) == "perspective")
			{
				//perspective camera defaults
				float fov = 1.0f;
				if(value.HasMember("fov") && value["fov"].IsNumber())
				{
					fov = read_float(value["fov"]);
				}
				return (Camera*)new PerspectiveCamera(origin, forward, right, fov);
			}
		}
	}
	throw std::exception("Could not create camera.");
}

template<typename T, typename V>
static Light *read_light(const GenericValue<T, V> &value)
{
	if(value.HasMember("point") && value["point"].IsObject())
	{
		glm::vec3 origin(0, 2, 0);
		glm::vec3 intensity(1.0, 1.0, 1.0);
		if(value["point"].HasMember("origin"))
		{
			origin = read_vec3(value["point"]["origin"]);
		}
		if(value["point"].HasMember("intensity"))
		{
			intensity = read_vec3(value["point"]["intensity"]);
		}
		return (Light*) new PointLight(origin, intensity);
	}
	throw std::exception("Could not read light.");
}

Scene::Scene(const char *filename)
	: camera(NULL)
{
	FILE *fp = fopen(filename, "rb");
	char buffer[0xffff];
	FileReadStream stream(fp, buffer, sizeof(buffer));
	Document doc;
	doc.ParseStream(stream);
	if(doc.HasMember("objects"))
	{
		auto &object_elem = doc["objects"];
		for(auto it = object_elem.Begin(); it != object_elem.End(); it++)
		{
			objects.push_back(read_object(*it));
		}
	}
	if(doc.HasMember("camera"))
	{
		if(doc["camera"].IsObject())
		{
			if(camera)
			{
				std::cerr << "Warning: multiple cameras specified, last seem will be used." << std::endl;
				delete camera;
			}
			camera = read_camera(doc["camera"]);
		}
		else
		{
			throw std::exception("invalid format for camera");
		}
	}
	if(doc.HasMember("lights"))
	{
		auto &object_elem = doc["lights"];
		for(auto it = object_elem.Begin(); it != object_elem.End(); it++)
		{
			lights.push_back(read_light(*it));
		}
	}
	else
	{
		camera = (Camera*) new PerspectiveCamera();
	}
	fclose(fp);
}

	
bool Scene::Intersects(const Ray &ray, Intersections::Record &info)
{
	Intersections::Record temp;
	info.t = std::numeric_limits<float>::max();
	bool found = false;
	for(auto object : objects)
	{
		if(object->Intersects(ray, info))
		{
			found = true;
			if(temp.t < info.t)
			{
				info = temp;
			}
		}
	}
	return found;
}


Colour Scene::Shade(const Ray &ray, const Intersections::Record &info, Scene &scene)
{
	return info.object->Shade(ray, info, scene);
}