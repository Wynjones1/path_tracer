#ifndef PLY_H
#define PLY_H
#include <string>
#include <map>
#include <fstream>

class Element
{
public:
	Element(uint32_t count = 0)
	: count(count)
	, list(false)
	{}

	std::vector<std::pair<std::string,std::vector<std::string>>> properties;
	std::vector<std::vector<std::string>> list_data;
	bool list;
	uint32_t count;

	std::string get_property(std::string property_name, uint32_t index)
	{
		for(auto &pair : properties)
		{
			if(pair.first == property_name)
			{
				return pair.second[index];
			}
		}
		throw std::exception("could not find property");
	}

	bool has_property(std::string property_name)
	{
		for(auto &pair : properties)
		{
			if(pair.first == property_name)
			{
				return true;
			}
		}
		return false;
	}
};

class PLY
{
public:
	PLY(std::string filename)
	{
		std::ifstream stream(filename);
		stream.exceptions(std::ios::eofbit);
		std::string input;
		std::string current_element;
		std::vector<std::string> element_order;
		uint32_t element_count;
		stream >> input;
		if(input != "ply")
		{
			throw std::exception("File is not a valid ply file.");
		}
		while(input != "end_header")
		{
			stream >> input;
			if(input == "comment")
			{
				char buffer[1024];
				stream.getline(buffer, 1024);
			}
			else if(input == "format")
			{
				stream >> input;
				if(input == "ascii")
				{
					format = ASCII;
				}
				else if(input == "binary")
				{
					format = BINARY;
				}
				else
				{
					throw std::exception("Invalid format");
				}
				stream >> version;
			}
			else if(input == "element")
			{
				stream >> current_element;
				stream >> element_count;
				elements[current_element] = Element(element_count);
				element_order.push_back(current_element);
			}
			else if(input == "property")
			{
				std::string property_type;
				std::string property_name;
				stream >> property_type;
				if(property_type == "list")
				{
					stream >> input; //count type
					stream >> input; //value type
					elements[current_element].list = true;
				}
				stream >> property_name; //data type
				auto temp = std::pair<std::string,std::vector<std::string>>();
				temp.first = property_name;
				elements[current_element].properties.push_back(temp);
			}
		}
		for(auto &elem_name : element_order)
		{
			auto &elem = elements[elem_name];
			for(uint32_t i = 0; i < elem.count; i++)
			{
				if(elem.list)
				{
					if(elem.properties.size() != 1)
					{
						throw std::exception("More than one property specified, one of which is a list.");
					}
					auto &prop = elem.properties[0];
					uint32_t count;
					stream >> count;
					elem.list_data.push_back(std::vector<std::string>());
					for(uint32_t i = 0; i < count; i++)
					{
						stream >> input;
						elem.list_data.back().push_back(input);
					}
				}
				else
				{
					for(auto &prop : elem.properties)
					{
						stream >> input;
						prop.second.push_back(input);
					}
				}
			}
		}
	}

	union data_types
	{
		float f;
		uint32_t ui32;
	};

	enum
	{
		ASCII, BINARY
	}format;

	std::string version;
	std::map<std::string, Element> elements;
};

#endif