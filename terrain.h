#pragma once

#include "drawable.h"
#include "texture.h"
#include "light_source.h"

struct cbuffer_input
{
	XMMATRIX MVP;
	XMMATRIX Model;
	XMFLOAT4 AmbientLight;
	XMFLOAT3 LightPosition;
};

struct terrain_vertex
{
	vec3	Position;
	vec3	Normal;
	vec2	TextureUVCoordinate;
	real32	IsHighlighted = 0.0f;
};

class terrain : public drawable
{
	private:

	std::vector<vec2> WorldCoordinate;
	std::vector<terrain_vertex> Vertices;
	std::vector<uint32> Indices;

	protected:

	uv_quad GetTextureCoordinateFromTileType(tile_type Type);

	public:

	terrain(int64 Width, int64 Height);

	void Draw() override;

	void UpdateTileHighlighResource(uint64 Width, int64 X, int64 Y, real32 IsHighlighted);
	void UpdateTileTypeResource(uint64 Width, int64 X, int64 Y, tile_type Type);

	std::vector<terrain_vertex> &GetVertexData();

	vec2 GetWorldCoordinate(uint64 Index);
};