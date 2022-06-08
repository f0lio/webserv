#pragma once

#include <map>
#include <string>

// each identifier must have a corresponding rule in directiveRulesMap<>

#define DIRECTIVE_ARG_TYPE_STRING "STRING"
#define DIRECTIVE_ARG_TYPE_NUMBER "NUMBER"
#define DIRECTIVE_ARG_TYPE_ANY "ANY"

namespace parser
{
    struct DirectiveRules
    {
        char *name;
        char *args_type; // NUMBER | STRING | ANY
        size_t min_args;
        size_t max_args;
        int occurrence;
        bool is_required;
    };

    enum Occurrence
    {
        OCCURENCE_FORBIDDEN = -2,
        OCCURENCE_MULTIPLE = -1,
        OCCURENCE_OPTIONAL = 0,
    };

    static const char *_context_identifiers[] = {
        "server"};

    static const char *_simple_identifiers[] = {
        "name",
        "listen",
        "root",
        "server_name",
        "error_page",
        "max_body_size",
        "methods",
    };

    static const char *_block_identifiers[] = {
        "location"};

    static const char *_location_identifiers[] = {
        "methods",
        "root",
        "index",
        "cgi",
    };

    static std::map<std::string, DirectiveRules>
        directiveRulesMap;

    static void initDirectiveRules()
    {
        directiveRulesMap["name"] = (DirectiveRules){
            .name = "name",
            .args_type = DIRECTIVE_ARG_TYPE_ANY,
            .min_args = 1,
            .max_args = 1,
            .occurrence = 1,
            .is_required = false};

        directiveRulesMap["listen"] = (DirectiveRules){
            .name = "listen",
            .args_type = DIRECTIVE_ARG_TYPE_ANY,
            .min_args = 1,
            .max_args = 2,
            .occurrence = OCCURENCE_MULTIPLE,
            .is_required = true};

        directiveRulesMap["root"] = (DirectiveRules){
            .name = "root",
            .args_type = DIRECTIVE_ARG_TYPE_STRING,
            .min_args = 1,
            .max_args = 1,
            .occurrence = 1,
            .is_required = true};

        directiveRulesMap["server_name"] = (DirectiveRules){
            .name = "server_name",
            .args_type = DIRECTIVE_ARG_TYPE_STRING,
            .min_args = 1,
            .max_args = 64, // monte carlo
            .occurrence = 1,
            .is_required = true};

        directiveRulesMap["error_page"] = (DirectiveRules){
            .name = "error_page",
            .args_type = DIRECTIVE_ARG_TYPE_STRING,
            .min_args = 2,
            .max_args = 2,
            .occurrence = OCCURENCE_MULTIPLE,
            .is_required = false};

        directiveRulesMap["max_body_size"] = (DirectiveRules){
            .name = "max_body_size",
            .args_type = DIRECTIVE_ARG_TYPE_NUMBER,
            .min_args = 1,
            .max_args = 1,
            .occurrence = 1,
            .is_required = false};

        directiveRulesMap["methods"] = (DirectiveRules){
            .name = "methods",
            .args_type = DIRECTIVE_ARG_TYPE_STRING,
            .min_args = 1,
            .max_args = 3,
            .occurrence = 1,
            .is_required = false};

        directiveRulesMap["location"] = (DirectiveRules){
            .name = "location",
            .args_type = DIRECTIVE_ARG_TYPE_STRING,
            .min_args = 1,
            .max_args = 1,
            .occurrence = 1024,
            .is_required = false};

        directiveRulesMap["index"] = (DirectiveRules){
            .name = "index",
            .args_type = DIRECTIVE_ARG_TYPE_STRING,
            .min_args = 1,
            .max_args = 24, // monte carlo
            .occurrence = 1,
            .is_required = true};

        directiveRulesMap["cgi"] = (DirectiveRules){
            .name = "cgi",
            .args_type = DIRECTIVE_ARG_TYPE_STRING,
            .min_args = 1,
            .max_args = 32, // monte carlo
            .occurrence = 1,
            .is_required = false};
    }
}
