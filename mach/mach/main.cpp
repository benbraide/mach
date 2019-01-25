#include "machine/machine_memory.h"
#include "machine/machine_register_table.h"

#include "parsing/ast.h"

int main(){
	mach::machine::register_table rt;

	MACH_AST_QNAME(mach_identifier) mi;
	mi.value = "";

	MACH_AST_DECLARE_SINGLE_VARIANT(sv, int, char, float);
	MACH_AST_NAME(sv) svi;
	svi.value = 9;

	MACH_AST_DECLARE_SINGLE(sv2, MACH_AST_VARIANT(int, char, float));
	MACH_AST_NAME(sv2) sv2i;
	sv2i.value = 9;

	return 0;
}
