#pragma once
#include "engine/core/types.h"
#include "engine/api/graphics_params.h"

namespace custom {

struct Asset {
	cstring path;
};

struct ShaderAsset : public Asset {
	graphics::Shader_Part parts;
};

struct TextureAsset : public Asset { };
struct MeshAsset : public Asset { };

}
