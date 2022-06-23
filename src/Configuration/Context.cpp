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

	void Context::check_occurrence(
		std::map<std::string, int>& loaded_directives, std::string const& key) const
	{
		loaded_directives[key]++;

		int occurrence = loaded_directives[key];
		int occurrence_rule = directiveRulesMap.at(key).occurrence;

		if (occurrence_rule != -1 && loaded_directives[key] > occurrence_rule)
		{
			throw std::runtime_error(
				"Directive \"" + key + "\" should occur "
				+ SSTR(occurrence_rule) 
				+ (occurrence_rule == 1 ? " time max" : " times max"));
		}
	}

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
					throw std::runtime_error(
						"Directive \"" + it->first + 
						"\" is required in " + _name + " context");
			}
		}

		std::map<std::string, int> loaded_directives;
		// dumb extra iteration for now
		std::vector<SimpleDirective>::iterator it_sdir;
		for (it_sdir = _simple_directives_vec.begin(); it_sdir != _simple_directives_vec.end(); ++it_sdir)
		{
			it_sdir->check_occurrence(loaded_directives, it_sdir->getKey());
			it_sdir->check();
		}
		std::vector<BlockDirective>::iterator it_bdir;
		for (it_bdir = _block_directives_vec.begin(); it_bdir != _block_directives_vec.end(); ++it_bdir)
			it_bdir->check();

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
