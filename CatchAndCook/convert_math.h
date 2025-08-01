#pragma once


#define D2R 0.017453292519943295f
#define R2D 57.29577951308232f

namespace std
{
    inline const float to_radian() { return D2R; }
    inline const float to_degree() { return R2D; }
    /// <summary>
    /// 부호 기준 -1, 0, 1
    /// </summary>
    /// <param names="x">int 값</param>
    /// <returns></returns>
    inline int sign(const int& x) { return (x > 0) - (x < 0); }
    inline double sign(const double& x) { return (x > 0) - (x < 0); }
    inline float sign(const float& x) { return (x > 0) - (x < 0); }
}
