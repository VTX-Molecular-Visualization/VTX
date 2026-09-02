#ifndef __VTX_TOOL_TOOL_STRUCTALIGN_BACKEND_ALIGN__
#define __VTX_TOOL_TOOL_STRUCTALIGN_BACKEND_ALIGN__

#include <core/struct/topology.hpp>
#include <util/types.hpp>

namespace VTX::Tool::Structalign::backend
{
	/**
	 * @brief struct representing a Structure to be aligned.
	 */
	struct Structure
	{
		std::reference_wrapper<Core::Struct::Topology> topology;
		std::span<Index>							   positions;
	};

	struct CEAlignParameters
	{
		Structure staticStructure, mobileStructure;
		float	  d0		 = 3.f;
		float	  d1		 = 4.f;
		uint32_t  windowSize = 8;
		uint32_t  maxGap	 = 30;
	};

	struct CEAlignResults
	{
		Mat4f transformMatrix;
		float rmsd = -1.f;
	};
} // namespace VTX::Tool::Structalign::backend
#endif
