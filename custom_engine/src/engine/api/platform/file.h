#pragma once
#include "engine/core/types.h"
#include "engine/api/internal/strings_storage.h"

namespace custom {
namespace file {

enum struct Action_Type : u8 {None, Add, Rem, Mod, Old, New};
struct Action { u32 id; Action_Type type; };

extern Strings_Storage strings;
extern Array<Action> actions;

u64 get_time(cstring path);
void read(cstring path, Array<u8> & buffer);

void watch_init(cstring path, bool subtree);
void watch_update(void);
void watch_shutdown(void);

}}
