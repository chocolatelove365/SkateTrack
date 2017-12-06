#include "converter.h"

void Converter::convert2Dto3D(Eigen::Matrix4d P, Eigen::Matrix4d Rt, const double x, const double y, const double Z, double &X, double &Y){
    Eigen::Matrix4d M = P * Rt;
    Eigen::Matrix4d A = M.inverse();
    Eigen::Matrix2d B;
    Eigen::Vector2d c, d;
    B << A(2, 2), A(2, 0)*x+A(2,1)*y+A(2, 3),
            A(3, 2), A(3, 0)*x+A(3,1)*y+A(3, 3);
    c << Z, 1.0;
    d = B.inverse() * c;
    double clipX = x * d[1];
    double clipY = y * d[1];
    double clipZ = d[0];
    double clipW = d[1];
    Eigen::Vector4d v, V;
    v << clipX, clipY, clipZ, clipW;
    V = A * v;
    X = V[0] / V[3];
    Y = V[1] / V[3];
}
