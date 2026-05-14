#include "time.hpp"

Time::Time(std::string timestamp) {
	// example : timestamp = "[10/13/2021 14:03:57.880805]"
	size_t space_idx = timestamp.find(" ");
	std::string s1 = timestamp.substr(1, space_idx - 1);
	std::string s2 = timestamp.substr(space_idx + 1, timestamp.length() - space_idx - 2);
	
	// example : s1 = "10/13/2021"
	t_.tm_mday = std::stoi(s1.substr(s1.find("/") + 1, s1.find_last_of("/") - s1.find("/") - 1));
	t_.tm_mon = std::stoi(s1.substr(0, s1.find("/"))) - 1; // Jan = 0, Feb = 1, ...
	t_.tm_year = std::stoi(s1.substr(s1.find_last_of("/") + 1)) - 1900; // years since 1900
	
	// example : s2 = "14:03:57.880805"
	t_.tm_hour = std::stoi(s2.substr(0, s2.find(":")));
	t_.tm_min = std::stoi(s2.substr(s2.find(":") + 1, s2.find_last_of(":") - s2.find(":") - 1));
	t_.tm_sec = (int) std::round(std::stod(s2.substr(s2.find_last_of(":") + 1)));
}

tm Time::GetTimeStruct() const {
	return t_;
}

double Time::operator-(Time& t_ref) {
	struct tm t0 = t_ref.GetTimeStruct();
	double elapsed = difftime(mktime(&t_), mktime(&t0));
	return elapsed;
}
