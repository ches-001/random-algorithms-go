#ifndef CUSTOM_TEMPLATES_H
#define CUSTOM_TEMPLATES_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "json.hpp"

template <typename T> using vec_t = std::vector<T>;
template <typename T> using mat_t = std::vector<std::vector<T>>;
template <typename keyT, typename VecValT> using uo_vec_map_t = std::unordered_map<keyT, std::vector<VecValT>>;
using json = nlohmann::json;

#endif