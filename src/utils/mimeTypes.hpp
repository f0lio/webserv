#pragma once

#include <string.h>

class mimeTypes
{
public:
	static const char *getType(const char *path);
	static const char *getExtension(const char *type);// int skip = 0);

private:
	struct entry
	{
		const char *fileExtension;
		const char *mimeType;
	};
	static const mimeTypes::entry types[346];
	static int strcmpi(const char *s1, const char *s2);
};