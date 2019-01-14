#pragma once

#include <vector>

#include "../io/io_writer.h"
#include "../machine/machine_op_code.h"
#include "../machine/machine_register_table.h"

namespace mach::asm_code{
	class instruction_operand{
	public:
		using byte = machine::register_object::byte;

		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const = 0;

		virtual machine::op_operand_type get_type() const = 0;

		virtual machine::op_operand_size get_size_type() const;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const = 0;
	};

	class regiester_instruction_operand : public instruction_operand{
	public:
		explicit regiester_instruction_operand(machine::register_object &reg);

		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const override;

		virtual machine::op_operand_type get_type() const override;

		virtual machine::op_operand_size get_size_type() const override;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const override;

	private:
		machine::register_object *reg_;
	};

	class memory_instruction_operand : public instruction_operand{
	public:
		memory_instruction_operand(std::shared_ptr<instruction_operand> key, machine::op_operand_size size_type);

		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const override;

		virtual machine::op_operand_type get_type() const override;

		virtual machine::op_operand_size get_size_type() const override;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const override;

	private:
		std::shared_ptr<instruction_operand> key_;
		machine::op_operand_size size_type_;
	};

	class immediate_instruction_operand : public instruction_operand{
	public:
		template <typename value_type>
		explicit immediate_instruction_operand(value_type value = value_type()){
			memcpy(data_, &value, sizeof(value_type));
		}

		immediate_instruction_operand(const byte *data, std::size_t size);

		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const override;

		virtual machine::op_operand_type get_type() const override;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const override;

		virtual byte *get_data() const;

		template <typename target_type>
		target_type get_data_as() const{
			auto value = target_type();
			memcpy(&value, data_, ((sizeof(target_type) < sizeof(unsigned __int64)) ? sizeof(target_type) : sizeof(unsigned __int64)));
		}

	private:
		mutable byte data_[sizeof(unsigned __int64)];
	};

	class offset_instruction_operand : public instruction_operand{
	public:
		struct part_info{
			machine::op_offset_operator op;
			std::shared_ptr<instruction_operand> operand;
		};

		explicit offset_instruction_operand(const std::vector<part_info> &parts);

		explicit offset_instruction_operand(std::vector<part_info> &&parts);

		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const override;

		virtual machine::op_operand_type get_type() const override;

		virtual machine::op_operand_size get_size_type() const override;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const override;

	private:
		virtual void resolve_size_type_();

		std::vector<part_info> parts_;
		machine::op_operand_size size_type_ = machine::op_operand_size::nil;
	};
}
