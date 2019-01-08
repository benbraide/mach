#include "io/io_binary_string_reader.h"
#include "machine/machine_register.h"

int main(){
	mach::machine::qword_integer_register qwir;
	auto qw = qwir.read_scalar<unsigned __int64>();
	auto dw = qwir.read_scalar<unsigned __int32>();
	auto w = qwir.read_scalar<unsigned __int16>();
	auto b = qwir.read_scalar<unsigned __int8>();

	qw = qwir.get_child()->read_scalar<unsigned __int64>();
	qw = qwir.get_child()->read_converted_scalar<unsigned __int64>();
	qw = qwir.get_child()->read_qword();
	dw = qwir.get_child()->read_scalar<unsigned __int32>();
	w = qwir.get_child()->read_scalar<unsigned __int16>();
	b = qwir.get_child()->read_scalar<unsigned __int8>();

	return 0;
}
