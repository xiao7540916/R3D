//
// Created by Administrator on 2022/9/20.
//
#pragma once
#include <structdefine.h>
#include <string>
#include <sstream>
#include <iostream>
#define RAND_MAX 9999
namespace R3D {
    using std::string;
    //生成范围内随机数
    inline int RandomInt(int from = 0, int to = 10) {
        int ran = rand() % (to - from + 1) + from;
        return ran;
    }
    inline float RandomFloat(float from = 0.0f, float to = 10.0f) {
        float baserand = float(rand() % RAND_MAX) / (float(RAND_MAX + 1));
        return from + baserand * (to - from);
    }
    //整数，浮点数转化为字符串
    inline string IntToString(float in_num) {
        std::stringstream ss;
        ss << in_num;
        string re;
        ss >> re;
        return re;
    }
    //字符串转化为整数
    inline int StringToInt(const string &in_str) {
        return atoi(in_str.c_str());
    }
    //字符串转化为浮点数
    inline float StringToFloat(const string &in_str) {
        return (float) atof(in_str.c_str());
    }
    //检测两个浮点数是否相等
    inline bool EqualFloat(float l, float r) {
        return abs(l - r) < EPSILON_E6;
    }
    inline float SquaredVec3(const vec3 &v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }
    //从路径里截取文件名称，如果只有文件名则直接返回(不包含后缀)
    inline string GetNameFromPath(const string &in_path) {
        size_t beg = in_path.find_last_of("\\/");
        size_t end = in_path.find_last_of(".");
        if (beg == string::npos) {
            //处理只有文件名情况
            beg = -1;
        }
        return in_path.substr(beg + 1, end - beg - 1);
    }
    //从路径里截取文件名称，如果只有文件名则直接返回(不包含后缀)
    inline string GetPathFromUrl(const string &in_path) {
        size_t beg = in_path.find_last_of("\\/");
        size_t end = in_path.find_last_of(".");
        if (beg == string::npos) {
            //处理只有文件名情况
            beg = -1;
        }
        return in_path.substr(0, beg);
    }
    //将给定字符串两端的空白字符删除
    inline string Trim(const string &in_str) {
        const static string SPACE_CHAR = "\t\f\v\n\r";
        size_t beg = in_str.find_first_not_of(SPACE_CHAR);

        //防止空字符
        if (beg > in_str.length()) {
            return string();
        }
        string result = in_str.substr(beg);
        size_t end = result.find_last_not_of(SPACE_CHAR);
        if (end != string::npos) {
            end++;
        }
        return result.substr(0, end);
    }
    //打印向量，矩阵
    inline void PrintVec3(const vec3 &toprint) {
        std::cout << toprint.x << " " << toprint.y << " " << toprint.z << std::endl;
    }
    inline void PrintMat3(const mat3 &toprint) {
        std::cout << toprint[0][0] << " " << toprint[0][1] << " " << toprint[0][2] << std::endl;
        std::cout << toprint[1][0] << " " << toprint[1][1] << " " << toprint[1][2] << std::endl;
        std::cout << toprint[2][0] << " " << toprint[2][1] << " " << toprint[2][2] << std::endl;
    }
    inline void PrintMat4(const mat4 &toprint) {
        std::cout << toprint[0][0] << " " << toprint[0][1] << " " << toprint[0][2] << " " << toprint[0][3] << std::endl;
        std::cout << toprint[1][0] << " " << toprint[1][1] << " " << toprint[1][2] << " " << toprint[1][3] << std::endl;
        std::cout << toprint[2][0] << " " << toprint[2][1] << " " << toprint[2][2] << " " << toprint[2][3] << std::endl;
        std::cout << toprint[3][0] << " " << toprint[3][1] << " " << toprint[3][2] << " " << toprint[3][3] << std::endl;
    }
}
