#pragma once

#include "Utils.hpp"
#include "Directive.hpp"
#include "ParserRules.hpp"

namespace parser
{
	class SimpleDirective;
	class BlockDirective;
	class Context
	{
	public:
		Context();
		~Context();

		std::string getName() const;

		void setName(std::string &name);

		void setIndex(size_t index); // used for setting default_server
		
		size_t getIndex() const;

		void addDirective(const BlockDirective &dir);

		void addDirective(const SimpleDirective &dir);

		std::vector<SimpleDirective> const &getSimpleDirectives() const;

		std::vector<BlockDirective> const &getBlockDirectives() const;

		// std::vector<std::string> get(std::string key)
		// {
		// 	for (size_t i = 0; i < _simple_directives_vec.size(); i++)
		// 		if (_simple_directives_vec[i].getKey() == key)
		// 			return _simple_directives_vec[i].getArgs();
		// 	return std::vector<std::string>();
		// }

		void prepare();

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
