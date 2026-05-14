#ifndef TIME_HPP
#define TIME_HPP

#include <string>
#include <time.h>
#include <cmath>

class Time {
public:
	Time(std::string);
	tm GetTimeStruct() const;
	double operator-(Time&);
private:
	struct tm t_ = {};
};

#endif
