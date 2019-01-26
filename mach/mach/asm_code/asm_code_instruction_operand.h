#pragma once

#include <string>
#include <vector>
#include <functional>

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

		virtual void validate_operands() const;

		virtual void resolve_operands();

		virtual void traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep = true) const;
	};

	class register_instruction_operand : public instruction_operand{
	public:
		explicit register_instruction_operand(machine::register_object &reg);

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
		enum class held_value_type{
			nil,
			char_,
			integral,
			float_,
		};

		immediate_instruction_operand();

		template <typename value_type>
		explicit immediate_instruction_operand(value_type value, held_value_type type = held_value_type::nil)
			: type_(type){
			set_data(value, type);
		}

		template <typename value_type>
		immediate_instruction_operand(value_type value, bool is_data_operand, held_value_type type = held_value_type::nil)
			: type_(type), is_data_operand_(is_data_operand){
			set_data(value, type);
		}

		immediate_instruction_operand(const byte *data, std::size_t size, held_value_type type);

		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const override;

		virtual machine::op_operand_type get_type() const override;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const override;

		virtual bool is_float_value() const;

		virtual held_value_type get_held_type() const;

		virtual byte *get_data() const;

		template <typename target_type>
		target_type get_data_as() const{
			if (std::is_integral_v<target_type> == is_float_value()){//Convert value
				if (std::is_integral_v<target_type>){//Convert from float to int
					long double value = 0;
					memcpy(&value, data_, sizeof(long double));
					return static_cast<target_type>(value);
				}

				unsigned __int64 value = 0;//Convert from int to float
				memcpy(&value, data_, sizeof(unsigned __int64));

				return static_cast<target_type>(value);
			}

			auto value = target_type();
			memcpy(&value, data_, ((sizeof(target_type) < sizeof(unsigned __int64)) ? sizeof(target_type) : sizeof(unsigned __int64)));

			return value;
		}

		template <typename value_type>
		void set_data(value_type value, held_value_type type = held_value_type::nil){
			memcpy(data_, &value, ((sizeof(value_type) < sizeof(unsigned __int64)) ? sizeof(value_type) : sizeof(unsigned __int64)));
			if (sizeof(value_type) < sizeof(unsigned __int64))//Zero higher bits
				memset((data_ + sizeof(value_type)), 0, (sizeof(unsigned __int64) - sizeof(value_type)));

			if (type == held_value_type::nil)//Resolve type
				type_ = (std::is_integral_v<value_type> ? held_value_type::integral : held_value_type::float_);
		}

	protected:
		mutable byte data_[sizeof(unsigned __int64)];
		held_value_type type_ = held_value_type::nil;
		bool is_data_operand_ = false;
	};

	class expression_instruction_operand : public immediate_instruction_operand{
	public:
		explicit expression_instruction_operand(char op, std::shared_ptr<instruction_operand> left, std::shared_ptr<instruction_operand> right);

		virtual void validate_operands() const override;

		virtual void resolve_operands() override;

		virtual void traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep = true) const override;

	protected:
		char op_;
		std::shared_ptr<instruction_operand> left_;
		std::shared_ptr<instruction_operand> right_;
	};

	class label_ref_instruction_operand : public immediate_instruction_operand{
	public:
		label_ref_instruction_operand(const std::string &name, bool is_data_operand);

		const std::string &get_name() const;

	protected:
		std::string name_;
	};

	class placeholder_instruction_operand : public label_ref_instruction_operand{
	public:
		explicit placeholder_instruction_operand(bool is_data_operand);
	};

	class uninitialized_instruction_operand : public instruction_operand{
	public:
		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const override;

		virtual machine::op_operand_type get_type() const override;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const override;
	};

	class offset_instruction_operand : public instruction_operand{
	public:
		struct part_info{
			machine::op_offset_operator op;
			std::shared_ptr<instruction_operand> operand;
		};

		offset_instruction_operand(const std::vector<part_info> &parts, bool is_explicit);

		offset_instruction_operand(std::vector<part_info> &&parts, bool is_explicit);

		virtual void encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const override;

		virtual machine::op_operand_type get_type() const override;

		virtual machine::op_operand_size get_size_type() const override;

		virtual std::size_t get_encoded_size(machine::op_operand_size size_type) const override;

		virtual void traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep = true) const override;

	private:
		virtual void resolve_size_type_();

		std::vector<part_info> parts_;
		machine::op_operand_size size_type_ = machine::op_operand_size::nil;
		bool is_explicit_;
	};
}
