#pragma once

#include "common.h"
#include "world.h"
#include "system_message.h"

#include <fstream>
#include <string>
#include <vector>

#define OBJECT_FILES_DEFAULT_PATH "data\\objects\\"

struct face
{
	uint32 Position;
	uint32 TextureCoordinate;
	uint32 Normal;
};

struct obj_file
{
	std::vector<vec3> Vertices;
	std::vector<vec3> Normals;
	std::vector<face> FaceIndices;
};

class persistence
{
	private:

	const char *WorldSaveFile = "data\\saved_map.citysim";

	const char NEWLINE = 10;
	const char SPACE = 32;
	const char TAB = 9;

	public:

	persistence() = default;
	~persistence() = default;

	void SaveWorldMap(world *World);
	void LoadSavedWorldMap(world *World);
	obj_file LoadObjectFile(std::string Filename);
};