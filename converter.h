#ifndef CONVERTER_H
#define CONVERTER_H

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

class Converter
{
public:
    static void convert2Dto3D(Eigen::Matrix4d P, Eigen::Matrix4d Rt, const double x, const double y, const double Z, double &X, double &Y);
};

#endif // CONVERTER_H
