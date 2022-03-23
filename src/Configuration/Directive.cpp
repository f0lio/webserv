

#include "./Directive.hpp"

namespace parser
{
    Directive::Directive(
        const std::string &key, const std::vector<std::string> &params)
        : _key(key), _params(params)
    {
    }

    Directive::Directive()
    {
    }

    Directive::~Directive()
    {
    }

    DirectiveType Directive::getType()
    {
        return _type;
    }

} // namespace parser
