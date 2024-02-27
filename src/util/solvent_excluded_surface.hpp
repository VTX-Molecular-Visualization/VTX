#ifndef __VTX_UTIL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_UTIL_SOLVENT_EXCLUDED_SURFACE__

#include "model/category.hpp"

namespace VTX::Util::SolventExcludedSurface
{
	using memory_size_type = size_t;

	const memory_size_type SMALL_SES_MEMORY_THRESHOLD = memory_size_type( 8000000000 ); // 8Go

	bool checkSESMemory( const Model::Category & category );

} // namespace VTX::Util::SolventExcludedSurface

#endif
