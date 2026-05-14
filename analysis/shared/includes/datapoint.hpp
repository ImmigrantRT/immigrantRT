#ifndef DATAPOINT_HPP
#define DATAPOINT_HPP

#include <limits>

struct DataPoint1 {
    double cpu;
    double mem;

    DataPoint1() : cpu(std::numeric_limits<double>::infinity()), mem(std::numeric_limits<double>::infinity()) {}
    DataPoint1(double c, double m) : cpu(c), mem(m) {}
};

struct DataPoint2 {
    DataPoint1 values;
    DataPoint1 intervals;

    DataPoint2(double c, double i1, double m, double i2) : values(c, m), intervals(i1, i2) {}
};

#endif