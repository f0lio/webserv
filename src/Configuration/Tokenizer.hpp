
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include <stdio.h> //printf

#define TOKEN_BLOCK_OPEN '{'
#define TOKEN_BLOCK_CLOSE '}'
#define TOKEN_SDIRECTIVE_CLOSE ';'
#define TOKEN_COMMENT '#'

namespace parser
{
	/*
		adding `Context` prototype cuz gcc stops at
		the very first namespace it faces, ig.
	*/
	class Context;
	enum TokenType
	{
		IDENTIFIER,
		PARAM_LITERAL, // STRING | NUMBER
		BLOCK_OPEN,
		BLOCK_CLOSE,
		SDIRECTIVE_END,
		COMMENT,
		END_OF_FILE
	};

	static const char *TokenTypeStrings[] = {
		"IDENTIFIER",
		"PARAM_LITERAL",
		"BLOCK_OPEN",
		"BLOCK_CLOSE",
		"SDIRECTIVE_END",
		"COMMENT",
		"END_OF_FILE"};

	class Token
	{
		friend class Tokenizer;
		friend class Parser; //!

	public:
		Token();
		Token(TokenType type, std::string value, size_t line);
		~Token();

	private:
		TokenType _type;
		std::string _value;
		size_t _line;
	};

	class Tokenizer
	{
		friend class Parser;

	public:
		Tokenizer(std::istream &stream);
		~Tokenizer();

		void print() const;
		std::vector<Token> getTokens();

	private:
		void tokenize();
		Token next();
		bool hasNext() const;
		Token peek() const;

		std::vector<Token> _tokens;
		std::istream &_stream;
		size_t _line_number;
		size_t _index;
	};
}
