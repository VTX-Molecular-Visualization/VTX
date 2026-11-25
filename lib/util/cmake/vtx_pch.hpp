#ifndef VTX_UTIL_PCH_HPP
#define VTX_UTIL_PCH_HPP

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
//
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
// Do not add spdlog, it includes fmt that uses "#pragma GCC optimize".
// GCC leaks the resulting __OPTIMIZE__ macro to every TU using the pch.
// #include <spdlog/spdlog.h>
// #include <fmt/format.h>
#endif
