#pragma once

#include "Context.hpp"


namespace parser
{
	Context::Context() {};
	Context::~Context() {};

	std::string Context::getName() const { return _name; }

	void Context::setName(std::string& name)
	{
		_name = name;
	}

	void Context::setIndex(size_t index) // used for setting default_server
	{
		_index = index;
	}

	size_t Context::getIndex() const
	{
		return _index;
	}

	void Context::addDirective(const BlockDirective& dir)
	{
		_block_directives_vec.push_back(dir);
	}

	void Context::addDirective(const SimpleDirective& dir)
	{
		_simple_directives_vec.push_back(dir);
	}

	std::vector<SimpleDirective> const& Context::getSimpleDirectives() const
	{
		return _simple_directives_vec;
	}

	std::vector<BlockDirective> const& Context::getBlockDirectives() const
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

	void Context::prepare()
	{
		std::map<std::string, DirectiveRules>::const_iterator it;
		for (it = directiveRulesMap.begin(); it != directiveRulesMap.end(); ++it)
		{
			if (it->second.is_required)
			{
				bool found = false;
				for (size_t i = 0; i < _simple_directives_vec.size(); i++)
				{
					if (_simple_directives_vec[i].getKey() == it->first)
					{
						found = true;
						break;
					}
				}
				if (!found)
					throw std::runtime_error("Directive \"" + it->first + "\" is required in context " + _name);
			}
		}

		for (it = locationDirectiveRulesMap.begin(); it != locationDirectiveRulesMap.end(); ++it)
		{
			if (it->second.is_required)
			{
				bool found = false;
				size_t i = 0;
				for (; i < _block_directives_vec.size(); i++)
				{
					for (size_t j = 0; j < _block_directives_vec[i].getDirectives().size(); j++)
					{
						if (_block_directives_vec[i].getDirectives()[j].getKey() == it->first)
						{
							found = true;
							break;
						}
					}
				}
				if (!found)
					throw std::runtime_error("Directive \"" + it->first + "\" is required in location block");
			}
		}
	}
} // namespace ws
