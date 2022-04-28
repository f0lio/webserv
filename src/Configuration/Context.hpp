#pragma once

#include <map>

#include <iostream>
#include <string>
#include "./Directive.hpp"

namespace parser
{
	class SimpleDirective;
	class BlockDirective;
	class Context
	{
	public:
		Context(){};
		~Context(){};

		std::string getName() const { return _name; }

		void setName(std::string &name)
		{
			_name = name;
		}

		void setIndex(size_t index) // used for setting default_server
		{
			_index = index;
		}

		size_t getIndex() const
		{
			return _index;
		}

		void addDirective(const BlockDirective &dir)
		{
			_block_directives_vec.push_back(dir);
		}

		void addDirective(const SimpleDirective &dir)
		{
			_simple_directives_vec.push_back(dir);
		}

		std::vector<SimpleDirective> const &getSimpleDirectives() const
		{
			return _simple_directives_vec;
		}

		std::vector<BlockDirective> const &getBlockDirectives() const
		{
			return _block_directives_vec;
		}

		// std::vector<std::string> get(std::string key)
		// {
		// 	for (size_t i = 0; i < _simple_directives_vec.size(); i++)
		// 		if (_simple_directives_vec[i].getKey() == key)
		// 			return _simple_directives_vec[i].getArgs();
		// 	return std::vector<std::string>();
		// }

		void prepare() {
			// for (size_t i = 0; i < _simple_directives_vec.size(); i++)
			// 	_simple_directives_vec[i].check();
			// for (size_t i = 0; i < _block_directives_vec.size(); i++)
			// 	_block_directives_vec[i].getDirectives();
			

		}

	private:
		std::string _name;
		size_t _index;
		std::map<std::string, SimpleDirective> _simple_directives;
		std::map<std::string, BlockDirective> _block_directives;

		// tempo
		std::vector<SimpleDirective> _simple_directives_vec;
		std::vector<BlockDirective> _block_directives_vec;
	};
} // namespace ws
