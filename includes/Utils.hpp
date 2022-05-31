#pragma once


#include <string>
#include <iostream>
#include <sstream>

#define ARRAY_SIZE(X_X) (sizeof(X_X) / sizeof(X_X[0]))

#define SSTR( x )	   static_cast< std::ostringstream & >( \
						( std::ostringstream() << std::dec << x  ) ).str()


#define PRINT_TOKEN(TOKEN) printf("%3.zu, %-15s=  [%s]\n",\
								TOKEN._line,\
								TokenTypeStrings[TOKEN._type],\
								TOKEN._value.c_str());

#define PASS 0