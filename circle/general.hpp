#pragma once

#include <vector>
#include <cmath>

typedef std::vector<std::vector<bool>> bitmap;
typedef std::vector<std::vector<float>> floatmap;


struct vec2{float x; float y;vec2(float _x, float _y):x(_x),y(_y){}

    vec2 operator*(float num) const { return vec2(x * num, y * num); }
	vec2 operator/(float num) const { return vec2(x / num, y / num); }
	vec2 operator-(const vec2& v) const { return vec2(x - v.x, y - v.y); }
	vec2 operator+(const vec2& v) const { return vec2(x + v.x, y + v.y); }

    inline float length() const { return sqrt(x * x + y * y); }
    inline vec2 normalize() const { return *this / length();}
    inline vec2 rotate() const {return vec2(y,-x);}
};
inline float dot(const vec2& v1, const vec2& v2) { return (v1.x * v2.x + v1.y * v2.y); }
//inline float angle(const vec2& v1, const vec2& v2) {return acos(dot(v1,v2)/(v1.length()*v2.length()));}

template<typename T>
inline std::vector<std::vector<T>> CreateMap(int a, int b)
{
    std::vector<std::vector<T>> result;
    for(int i=0;i<a;i++){
        std::vector<T> p1;
        result.push_back(p1);
        for(int j=0;j<b;j++){
            T d;
            result[i].push_back(d);
        }
        
    }

    return result;
}

inline bool FE(const float& f1, const float& f2, const float& tures = 0.0001f){return fabs(f1-f2)<tures;}
