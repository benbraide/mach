#include "machine/machine_memory.h"

int main(){
	char buffer[1024] = {};

	mach::machine::memory mm;
	auto blk = mm.allocate_block(sizeof(int) * 18);
	auto bw = mm.write_buffer(blk->get_address(), "Leading bytes", strlen("Leading bytes"));
	bw = mm.write_buffer((blk->get_address() + strlen("Leading bytes")), "Next bytes", strlen("Next bytes"));
	bw = mm.read_buffer(blk->get_address(), buffer, strlen("Leading bytes"));
	bw = mm.read_buffer(blk->get_address() + strlen("Leading bytes"), buffer, strlen("Next bytes"));

	return 0;
}
