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
		Context() {};
		~Context() {};
		std::string getName() const { return _name; }
		void setName(std::string &name) { _name = name; }
		void addDirective(const BlockDirective &dir)
		{
			_block_directives_vec.push_back(dir);
		}
		void addDirective(const SimpleDirective &dir)
		{
			_simple_directives_vec.push_back(dir);
		}

		std::vector<SimpleDirective> getSimpleDirectives()
		{
			return _simple_directives_vec;
		}

		std::vector<BlockDirective> getBlockDirectives()
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

	private:
		std::string _name;
		std::map<std::string, SimpleDirective> _simple_directives;
		std::map<std::string, BlockDirective> _block_directives;

		// tempo
		std::vector<SimpleDirective> _simple_directives_vec;
		std::vector<BlockDirective> _block_directives_vec;
	};
} // namespace ws
