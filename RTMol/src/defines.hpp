#ifndef __DEFINES_HPP__
#define __DEFINES_HPP__

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <string>

// Types.
using uint	= unsigned int;
using uchar = unsigned char;
using Vec2f = glm::vec2;
using Vec3f = glm::vec3;
using Vec4f = glm::vec4;

constexpr uint INVALID_ID = UINT_MAX;

// Image.
constexpr uint JPG_CHANNELS = 3u;
constexpr uint JPG_QUALITY	= 100u;

// Log.
inline void __OUT__( const std::string & str ) { std::cout << str << std::endl; }
inline void __ERR__( const std::string & str ) { std::cerr << str << std::endl; }

// Colors.
const Vec3f COLOR_BLACK		 = Vec3f( 0.0f, 0.0f, 0.0f );
const Vec3f COLOR_WHITE		 = Vec3f( 1.0f, 1.0f, 1.0f );
const Vec3f COLOR_GREY		 = Vec3f( 0.5f, 0.5f, 0.5f );
const Vec3f COLOR_RED		 = Vec3f( 1.0f, 0.13f, 0.0f );
const Vec3f COLOR_BLUE		 = Vec3f( 0.13f, 0.2f, 1.0f );
const Vec3f COLOR_GREEN_DARK = Vec3f( 0.f, 0.47f, 0.f );
const Vec3f COLOR_VIOLET	 = Vec3f( 0.f, 0.47f, 0.f );
const Vec3f COLOR_ORANGE	 = Vec3f( 1.f, 0.5f, 0.f );
const Vec3f COLOR_YELLOW	 = Vec3f( 1.f, 0.9f, 0.13f );
const Vec3f COLOR_MAGENTA	 = Vec3f( 1.f, 0.f, 1.f );

#endif
