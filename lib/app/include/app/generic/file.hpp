#ifndef VTX_APP_GENERIC_FILE_HPP
#define VTX_APP_GENERIC_FILE_HPP

#include <util/filesystem.hpp>

namespace VTX::App::Generic
{
	/**
	 * @brief Basic component for file entities.
	 */
	struct File
	{ FilePath path; };
} // namespace VTX::App::Generic

#endif
