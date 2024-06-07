#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__

#include <optional>

namespace VTX::Tool::Mdprep::Gateway
{

	struct GromacsStep
	{
		uint64_t				nstvout	  = 1000;
		uint64_t				nstenergy = 1000;
		uint64_t				nstlog	  = 1000;
		std::optional<uint64_t> nstxout_compressed;
	};
	struct GromacsData
	{
		std::string protonationScript;
		GromacsStep nvt;
		GromacsStep npt;
		GromacsStep prod { .nstxout_compressed = 100 };
	};

	void boxShapeCollection( std::vector<std::string> & ) noexcept;
	void waterModelCollection( std::vector<std::string> & ) noexcept;
} // namespace VTX::Tool::Mdprep::Gateway
#endif
