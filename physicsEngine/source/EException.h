#pragma once
#include <string>
class EException : public std::exception
{
protected:
	std::string msg;
	int line;
	std::string file;
	bool full;
public:
	EException();
	EException(const std::string wwh);
	EException(const std::string wwh, std::string File, int Line);
	virtual void CallCatch() = 0;
};