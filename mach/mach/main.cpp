#include "io/io_binary_string_reader.h"
#include "machine/machine_stack.h"

int main(){
	mach::machine::qword_integer_register qwir;
	qwir.write_converted_scalar(450);

	mach::machine::stack::byte buffer[1024];
	mach::machine::stack stk(buffer, 1024);

	stk.push(qwir);
	qwir.write_converted_scalar(720);
	stk.pop(qwir);

	return 0;
}
