#pragma once

#include <string>
#include <unordered_map>

#include "machine_register.h"

namespace mach::machine{
	class register_table{
	public:
		struct cache_info{
			register_object *base_pointer;
			register_object *stack_pointer;
			register_object *instruction_pointer;
		};

		register_table();

		register_object *find(const std::string &key) const;

		register_object *get_base_pointer() const;

		register_object *get_stack_pointer() const;

		register_object *get_instruction_pointer() const;

		void set_flag(unsigned int flag);

		void clear_flag(unsigned int flag);

		bool has_flag(unsigned int flag) const;

		static const unsigned int flag_nil			= (0u << 0x0000u);
		static const unsigned int flag_zero			= (1u << 0x0000u);
		static const unsigned int flag_less			= (1u << 0x0001u);

	private:
		cache_info cache_{};
		unsigned int flags_ = flag_nil;
		std::unordered_map<std::string, std::shared_ptr<register_object>> registers_;
	};
}
