/*
 * File: ioUtils.cpp
 * Author: Maxime MARIA
 */

#ifndef __IO_UTILS_HPP__
#define __IO_UTILS_HPP__

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "glm/glm.hpp"

#include "appConfig.hpp"
#include "defines.hpp"
#include "io/filePath.hpp"
#include "math/aabb.hpp"

namespace vidocklab
{
	static inline std::ostream &operator<<(std::ostream &os, const Vec2f &vec)
	{
		os << "[" << vec.x << ", " << vec.y << "]";
		return os;
	}

	static inline std::ostream &operator<<(std::ostream &os, const Vec3f &vec)
	{
		os << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
		return os;
	}

	static inline std::ostream &operator<<(std::ostream &os, const Vec4f &vec)
	{
		os << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w
		   << "]";
		return os;
	}

	static inline std::ostream &operator<<(std::ostream &os, const Mat4f &m)
	{
		os << "|" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", "
		   << m[0][3] << "|" << std::endl;
		os << "|" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", "
		   << m[1][3] << "|" << std::endl;
		os << "|" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", "
		   << m[2][3] << "|" << std::endl;
		os << "|" << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", "
		   << m[3][3] << "|" << std::endl;
		return os;
	}

	static inline std::ostream &operator<<(std::ostream &os, const AABB &aabb)
	{
		os << "[" << aabb.getMin() << ", " << aabb.getMax();
		return os;
	}

	static inline std::string readFile(const FilePath &filePath)
	{
		std::ifstream ifs(filePath, std::ifstream::in);
		if (!ifs.is_open())
			throw std::ios_base::failure("cannot open file: " + filePath.str());

		std::stringstream s;
		s << ifs.rdbuf();
		ifs.close();
		return s.str();
	}

	class VerboseOStream
	{
	public:
		VerboseOStream(std::ostream &os) : m_os(os) {}
		template<typename T>
		const VerboseOStream &operator<<(const T &vos) const
		{
			if (AppConfig::verbose)
				m_os << vos;
			return *this;
		}

		const VerboseOStream &operator<<(
			std::ostream &(*f)(std::ostream &)) const
		{
			if (AppConfig::verbose)
				f(m_os);
			return *this;
		}

		static VerboseOStream cout;

	private:
		std::ostream &m_os;
	};

	namespace StringFormat
	{
		static inline void removeSpaces(std::string &str)
		{
			str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
		}

		static inline void toupper(std::string &str)
		{
			std::transform(
				str.begin(), str.end(), str.begin(),
				[](unsigned char c) { return char(std::toupper(c)); });
		}
	} // namespace StringFormat
} // namespace vidocklab

#endif // __IO_UTILS_HPP__
