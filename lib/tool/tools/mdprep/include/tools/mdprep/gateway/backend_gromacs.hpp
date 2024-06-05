#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__

#include <optional>

namespace VTX::Tool::Mdprep::Gateway
{
	class GromacsFieldPlacer;

	struct GromacsStep
	{
		uint64_t				nstvout	  = 1000;
		uint64_t				nstenergy = 1000;
		uint64_t				nstlog	  = 1000;
		std::optional<uint64_t> nstxout_compressed;
	};
	struct GromacsData
	{
		GromacsStep nvt;
		GromacsStep npt;
		GromacsStep prod { .nstxout_compressed = 100 };
	};

} // namespace VTX::Tool::Mdprep::Gateway
#endif
