#pragma once
#include "MetaTypes.h"


class ErrorLog
{
public:
	static const char *LogDir;
	static const char *LogFileName;

	static void AppendError(char *entry)
	{
		AppendError("ERROR", entry);
	}
	static void AppendError(char *type, char *entry);
};

