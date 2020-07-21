#pragma once

namespace custom {
struct Bytecode;
}

namespace custom {
namespace graphics {

void init(void);
void shutdown(void);
void consume(Bytecode const & bc);

}}
