#ifndef PLY_H
#define PLY_H
#include <string>
#include <map>
#include <fstream>
#include <array>
#include <algorithm>

union ply_data_t
{
	float    f;
	uint32_t i;
};

enum ply_data_types
{
	PLY_DATA_TYPE_FLOAT,
	PLY_DATA_TYPE_UINT32,
};

class Element
{
public:
	Element(uint32_t count = 0)
	: count(count)
	, list(false)
	{}

	std::map<std::string, ply_data_types>          types;
	std::map<std::string, std::vector<ply_data_t>> properties;
	std::vector<std::vector<ply_data_t>>           list_data;
	std::vector<std::string>                       property_order;
	bool list;
	uint32_t count;

	ply_data_t get_property(std::string property_name, uint32_t index)
	{
		return properties[property_name][index];
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
		try
		{
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
					auto &elem = elements[current_element];
					elem.properties[property_name] = std::vector<ply_data_t>();
					if( property_type == "char" || property_type == "uchar"  ||
						property_type == "short"|| property_type == "ushort" ||
						property_type == "int"  || property_type == "uint")
					{
						elem.types[property_name] = PLY_DATA_TYPE_UINT32;
					}
					else if(property_type == "float" || property_type == "double")
					{
						elem.types[property_name] = PLY_DATA_TYPE_FLOAT;
					}
					else if(property_type == "list")
					{
						elem.types[property_name] = PLY_DATA_TYPE_UINT32;
					}
					else
					{
						throw std::exception("invalid data type in ply file");
					}
					elem.property_order.push_back(property_name);
					elem.properties[property_name].resize(elem.count);
				}
			}
			/* read the element data */
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
						auto &prop = elem.property_order[0];
						uint32_t count;
						stream >> count;
						elem.list_data.push_back(std::vector<ply_data_t>());
						elem.list_data.back().resize(count);
		
						for(uint32_t j = 0; j < count; j++)
						{
							ply_data_types t = elem.types[prop];
							switch(t)
							{
							case PLY_DATA_TYPE_FLOAT:
								stream >> elem.list_data[i][j].f;
								break;
							case PLY_DATA_TYPE_UINT32:
								stream >> elem.list_data[i][j].i;
								break;
							}
						}
					}
					else
					{
						for(auto &prop : elem.property_order)
						{
							ply_data_types t = elem.types[prop];
							switch(t)
							{
							case PLY_DATA_TYPE_FLOAT:
								stream >> elem.properties[prop][i].f;
								break;
							case PLY_DATA_TYPE_UINT32:
								stream >> elem.properties[prop][i].i;
								break;
							}
						}
					}
				}
			}
		}
		catch(std::ios_base::failure &ex)
		{
			std::cerr << "exception reading file file: " << filename << ":" << ex.what() << std::endl;
		}
		catch(std::exception &ex)
		{
			std::cerr << "exception reading file file: " << filename << ":" << ex.what() << std::endl;
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