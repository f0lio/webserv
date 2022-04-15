
#include "Configuration.hpp"

namespace ws
{

    Configuration::Configuration(const std::string &path) : _path(path)
    {
        ;
    }

    Configuration::~Configuration()
    {

    }

    void Configuration::parse()
    {
        std::ifstream file(_path.c_str());
        if (!file.is_open())
            throw std::runtime_error("Can't open file: " + _path);

        parser::Tokenizer tokenizer(file);
        parser::Parser parser(tokenizer);

        parser.parse();
        parser.print();
        file.close();
    }

    void Configuration::print() const
    {
        std::cout << "Printing.." << std::endl;
    }

    std::vector<parser::Context> Configuration::getContexts() const
    {
        return _servers;
    }
} // namespace ws
