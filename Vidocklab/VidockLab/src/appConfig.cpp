/*
 * File: appConfig.cpp
 * Author: Maxime MARIA
 */

#include "appConfig.hpp"

namespace vidocklab
{
	bool AppConfig::verbose = true;

	FilePath AppConfig::exeDirPath;
	FilePath AppConfig::shadersDirPath;

	bool AppConfig::vSync = false;

	const std::string AppConfig::glslVersion = "#version 450";
} // namespace vidocklab
