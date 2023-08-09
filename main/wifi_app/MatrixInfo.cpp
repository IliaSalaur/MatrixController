#include "MatrixInfo.hpp"

MatrixInfo:: MatrixInfo()
    :
    id{0},
    ip{0},
    width{0},
    height{0},
    lastTimePing{-1},
    tag{""}
{}

MatrixInfo::MatrixInfo(uint32_t id_, uint32_t ip_, size_t width_, size_t height_, int64_t lastPing_, const std::string& tag_)
    :
    id{id_},
    ip{ip_},
    width{width_},
    height{height_},
    lastTimePing{lastPing_},
    tag{tag_}
{}

// void MatrixInfo::to_json(nlohmann::json& nlohmann_json_j, const MatrixInfo& nlohmann_json_t)
// {
//     nlohmann_json_j["id"] = nlohmann_json_t.id; 
//     nlohmann_json_j["ip"] = nlohmann_json_t.ip; 
//     nlohmann_json_j["width"] = nlohmann_json_t.width; 
//     nlohmann_json_j["height"] = nlohmann_json_t.height; 
//     nlohmann_json_j["tag"] = nlohmann_json_t.tag; 
// }

// void MatrixInfo::from_json(const nlohmann::json& nlohmann_json_j, MatrixInfo& nlohmann_json_t)
// { 
//     nlohmann_json_j.at("id").get_to(nlohmann_json_t.id); 
//     nlohmann_json_j.at("ip").get_to(nlohmann_json_t.ip); 
//     nlohmann_json_j.at("width").get_to(nlohmann_json_t.width); 
//     nlohmann_json_j.at("height").get_to(nlohmann_json_t.height); 
//     nlohmann_json_j.at("tag").get_to(nlohmann_json_t.tag); 
// }

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MatrixInfo, id, ip, width, height, tag)

// list holding all matrices
std::list<MatrixInfo> http_server_matrices{};