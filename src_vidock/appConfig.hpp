/*
 * File: appConfig.hpp
 * Author: Maxime MARIA
 */

#ifndef __APP_CONFIG_HPP__
#define __APP_CONFIG_HPP__

#include <string>

#include "defines.hpp"

#include "io/filePath.hpp"

namespace vidocklab
{
	struct AppConfig
	{
		static bool verbose;

		static FilePath exeDirPath;
		static FilePath shadersDirPath;

		static bool vSync;

		static const std::string glslVersion;
	};
} // namespace vidocklab

#endif // __APP_CONFIG_HPP__
