#pragma once

#include <map>
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
		void setName(std::string &name) { _name = name; }
		void addDirective(const BlockDirective &dir)
		{
			_block_directives_vec.push_back(dir);
		}
		void addDirective(const SimpleDirective &dir)
		{
			// _simple_directives[dir.getKey()] = dir;
			_simple_directives_vec.push_back(dir);
		}

		std::vector<SimpleDirective> getSimpleDirectives() {
			return _simple_directives_vec;
		}
		std::vector<BlockDirective> getBlockDirectives() {
			return _block_directives_vec;
		}

	private:
		std::string _name;
		std::map<std::string, SimpleDirective> _simple_directives;
		std::map<std::string, BlockDirective> _block_directives;

		// tempo
		std::vector<SimpleDirective> _simple_directives_vec;
		std::vector<BlockDirective> _block_directives_vec;
	};
} // namespace ws
