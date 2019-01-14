#pragma once

#include "asm_code_instruction_operand.h"

namespace mach::asm_code{
	enum class instruction_error_code{
		nil,
		ambiguous_instruction,
		bad_operand,
		bad_operand_count,
		mismatched_operand_types,
	};

	class instruction{
	public:
		using byte = instruction_operand::byte;

		explicit instruction(const std::vector<std::shared_ptr<instruction_operand>> &operands);

		explicit instruction(std::vector<std::shared_ptr<instruction_operand>> &&operands);

		virtual void encode(io::writer &writer, machine::register_table &reg_table) const;

		virtual machine::op_code get_op_code() const = 0;

		virtual machine::op_operand_size get_size_type() const;

		virtual std::size_t get_encoded_size() const;

		virtual void validate_operands() const;

	protected:
		virtual void resolve_size_type_();

		virtual std::size_t get_required_operand_count_() const;

		virtual bool is_valid_size_type_(machine::op_operand_size size_type) const;

		virtual bool is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const;

		machine::op_operand_size size_type_;
		std::vector<std::shared_ptr<instruction_operand>> operands_;
	};

	template <machine::op_code op_code>
	class instruction_with_no_operands : public instruction{
	public:
		instruction_with_no_operands()
			: instruction({}){}

		virtual machine::op_code get_op_code() const override{
			return op_code;
		}

	protected:
		virtual std::size_t get_required_operand_count_() const override{
			return 0u;
		}
	};

	using nop_instruction		= instruction_with_no_operands<machine::op_code::nop>;
	using leave_instruction		= instruction_with_no_operands<machine::op_code::leave>;
	using ret_instruction		= instruction_with_no_operands<machine::op_code::ret>;
	using syscall_instruction	= instruction_with_no_operands<machine::op_code::syscall>;

	class mov_instruction : public instruction{
	public:
		explicit mov_instruction(const std::vector<std::shared_ptr<instruction_operand>> &operands);

		explicit mov_instruction(std::vector<std::shared_ptr<instruction_operand>> &&operands);

		virtual machine::op_code get_op_code() const override;

	protected:
		virtual std::size_t get_required_operand_count_() const override;

		virtual bool is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const;
	};
}
