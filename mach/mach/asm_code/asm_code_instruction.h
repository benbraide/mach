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

	enum class decl_type{
		db,
		dw,
		dd,
		dq,
	};

	class instruction{
	public:
		using byte = instruction_operand::byte;
		using operand_type = std::vector<std::shared_ptr<instruction_operand>>;
		using size_pair_type = std::pair<std::size_t, std::size_t>;

		explicit instruction(const operand_type &operands);

		explicit instruction(operand_type &&operands);

		virtual void print(io::stream_writer &writer) const;

		virtual void print_meta(io::stream_writer &writer) const;

		virtual void print_operands(io::stream_writer &writer) const;

		virtual void encode(io::writer &writer, machine::register_table &reg_table) const;

		virtual void encode_meta(io::writer &writer) const;

		virtual void encode_operands(io::writer &writer, machine::register_table &reg_table) const;

		virtual machine::op_code get_op_code() const = 0;

		virtual machine::op_operand_size get_size_type() const;

		virtual std::size_t get_encoded_size() const;

		virtual std::size_t get_meta_encoded_size() const;

		virtual std::size_t get_operands_encoded_size() const;

		virtual void validate_operands() const;

		virtual void resolve_operands();

		virtual void traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep = true) const;

		virtual bool is_data_instruction() const;

		static std::string get_op_code_string(machine::op_code op_code);

	protected:
		virtual void resolve_size_type_();

		virtual size_pair_type get_required_operand_count_() const;

		virtual bool is_valid_size_type_(machine::op_operand_size size_type) const;

		virtual bool is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const;

		machine::op_operand_size size_type_;
		operand_type operands_;
	};

	template <machine::op_code op_code>
	class instruction_with_no_operands : public instruction{
	public:
		instruction_with_no_operands()
			: instruction({}){}

		explicit instruction_with_no_operands(const operand_type &operands)
			: instruction(operands){}

		explicit instruction_with_no_operands(operand_type &&operands)
			: instruction(std::move(operands)){}

		virtual machine::op_code get_op_code() const override{
			return op_code;
		}
	};

	using nop_instruction		= instruction_with_no_operands<machine::op_code::nop>;
	using leave_instruction		= instruction_with_no_operands<machine::op_code::leave>;
	using ret_instruction		= instruction_with_no_operands<machine::op_code::ret>;
	using syscall_instruction	= instruction_with_no_operands<machine::op_code::syscall>;

	template <machine::op_code op_code>
	class std_instruction : public instruction{
	public:
		explicit std_instruction(const operand_type &operands)
			: instruction(operands){}

		explicit std_instruction(operand_type &&operands)
			: instruction(std::move(operands)){}

		virtual machine::op_code get_op_code() const override{
			return op_code;
		}

	protected:
		virtual size_pair_type get_required_operand_count_() const override{
			return size_pair_type{ 2u, 2u };
		}

		virtual bool is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const override{
			switch (operand_type){
			case machine::op_operand_type::reg:
			case machine::op_operand_type::mem:
			case machine::op_operand_type::mem_with_offset:
				return true;
			case machine::op_operand_type::val:
				return (index == 1u);
			default:
				break;
			}

			return false;
		}
	};

	using mov_instruction		= std_instruction<machine::op_code::mov>;
	using cmp_instruction		= std_instruction<machine::op_code::cmp>;
	using test_instruction		= std_instruction<machine::op_code::test>;

	class data_instruction : public instruction{
	public:
		explicit data_instruction(const operand_type &operands);

		explicit data_instruction(operand_type &&operands);

		virtual machine::op_code get_op_code() const override;

		virtual bool is_data_instruction() const override;
	};

	template <decl_type type>
	class decl_instruction : public data_instruction{
	public:
		explicit decl_instruction(const operand_type &operands)
			: data_instruction(operands){}

		explicit decl_instruction(operand_type &&operands)
			: data_instruction(std::move(operands)){}

		virtual void print_meta(io::stream_writer &writer) const override{
			switch (type){
			case decl_type::db:
				writer.write("db", 2u);
				break;
			case decl_type::dw:
				writer.write("dw", 2u);
				break;
			case decl_type::dd:
				writer.write("dd", 2u);
				break;
			case decl_type::dq:
				writer.write("dq", 2u);
				break;
			default:
				break;
			}
		}

		virtual machine::op_operand_size get_size_type() const override{
			switch (type){
			case decl_type::db:
				return machine::op_operand_size::byte;
			case decl_type::dw:
				return machine::op_operand_size::word;
			case decl_type::dd:
				return machine::op_operand_size::dword;
			case decl_type::dq:
				return machine::op_operand_size::qword;
			default:
				break;
			}

			return machine::op_operand_size::nil;
		}

	protected:
		virtual size_pair_type get_required_operand_count_() const override{
			return size_pair_type{ 1u, static_cast<std::size_t>(-1) };
		}

		virtual bool is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const override{
			return (operand_type == machine::op_operand_type::val);
		}
	};

	using db_instruction		= decl_instruction<decl_type::db>;
	using dw_instruction		= decl_instruction<decl_type::dw>;
	using dd_instruction		= decl_instruction<decl_type::dd>;
	using dq_instruction		= decl_instruction<decl_type::dq>;

	class zero_instruction : public data_instruction{
	public:
		explicit zero_instruction(unsigned __int64 size);

		virtual void print_meta(io::stream_writer &writer) const override;

		virtual void encode_operands(io::writer &writer, machine::register_table &reg_table) const;

		virtual machine::op_operand_size get_size_type() const override;

		virtual std::size_t get_operands_encoded_size() const override;

	protected:
		unsigned __int64 size_;
	};

	class string_instruction : public data_instruction{
	public:
		explicit string_instruction(const std::string &data);

		explicit string_instruction(std::string &&data);

		virtual void print_meta(io::stream_writer &writer) const override;

		virtual void encode_operands(io::writer &writer, machine::register_table &reg_table) const;

		virtual machine::op_operand_size get_size_type() const override;

		virtual std::size_t get_operands_encoded_size() const override;

	protected:
		std::string data_;
	};
}
