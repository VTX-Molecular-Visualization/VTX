#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__

#include <optional>

namespace VTX::Tool::Mdprep::Gateway
{

	// Hold parameters for an md step
	struct GromacsStep
	{
		uint64_t				nstvout	  = 1000;
		uint64_t				nstenergy = 1000;
		uint64_t				nstlog	  = 1000;
		std::optional<uint64_t> nstxout_compressed;
	};
	// Hold data used for system preparation
	struct GromacsSystem
	{
		size_t				  boxSizeMode = 0;
		double				  distanceNm  = 1.;
		std::array<double, 3> boxDimensions { 90., 90., 90. };
		std::array<double, 3> boxAngles { 90., 90., 90. };
	};
	// Hold all MDprep data specific to gromacs
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
