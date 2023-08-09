#ifndef MATRIX_INFO_HPP
#define MATRIX_INFO_HPP

#include <cstdint>
#include <string>
#include <list>

#include "nlohmann/json.hpp"

struct MatrixInfo
{
    uint32_t id;
    uint32_t ip;
    size_t width;
    size_t height;  
    int64_t lastTimePing;  
    std::string tag;

    MatrixInfo();
    MatrixInfo(uint32_t id_, uint32_t ip_, size_t width_, size_t height_, int64_t lastPing_, const std::string& tag_);

    // friend void to_json(nlohmann::json& nlohmann_json_j, const MatrixInfo& nlohmann_json_t);
    // friend void from_json(const nlohmann::json& nlohmann_json_j, MatrixInfo& nlohmann_json_t);
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MatrixInfo, id, ip, width, height, tag)
};
#endif