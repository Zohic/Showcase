#include "EException.h"

EException::EException()
{
	msg = "empty error";
}

EException::EException(const std::string wwh)
{
	msg = wwh;
	full = false;
}

EException::EException(const std::string wwh, std::string File, int Line)
{
	msg = wwh;
	file = File;
	line = Line;
	full = true;
}