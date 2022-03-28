
#include "Configuration.hpp"

namespace ws
{

    Configuration::Configuration(const std::string &path) : _path(path)
    {
        // valid_keys.insert(valid_keys.end(), valid_keys_, valid_keys_ + sizeof(valid_keys_) / sizeof(valid_keys_[0]));
        ;
    };

    Configuration::~Configuration(){};

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
        // parser::Parser parser(tokenizer.getTokens());
    
    }

    void Configuration::print() const
    {
        std::cout << "Printing.." << std::endl;
    }
} // namespace ws
