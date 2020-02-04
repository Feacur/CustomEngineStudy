#pragma once
#include "engine/core/types.h"

namespace custom {
struct Bytecode;
}

namespace custom {
namespace loader {

void init(Bytecode * bytecode);

void image(u32 asset_id);
void imagef(u32 asset_id);
void image16(u32 asset_id);

void shader(u32 asset_id);

void quad(u32 asset_id);
void particle_test(u32 asset_id);

}}
