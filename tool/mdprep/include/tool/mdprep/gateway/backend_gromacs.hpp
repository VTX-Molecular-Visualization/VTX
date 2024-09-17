#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_BACKEND_GROMACS__

#include <array>
#include <optional>
#include <stack>
#include <string>
#include <thread>
#include <util/sentry.hpp>
#include <vector>

namespace std
{
	class thread;
}
namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonInformation;
}
namespace VTX::Tool::Mdprep::Gateway::Gromacs
{
	enum class E_BOX_SHAPE
	{
		dodecahedron,
		octahedron,
		cubic,
		triclinic,
		COUNT
	};
	enum class E_BOX_DIMENSION_MODE
	{
		distance // uses the distance from the solute to the box to determine box's size
		,
		size // uses user-provided dimensions for the box size.
		,
		COUNT
	};

	// Hold parameters for an md step
	struct MdStep
	{
		uint64_t				nstvout	  = 1000;
		uint64_t				nstenergy = 1000;
		uint64_t				nstlog	  = 1000;
		std::optional<uint64_t> nstxout_compressed;
	};
	// Hold data used for system preparation
	struct SystemSettings
	{
		size_t				  boxSizeMode = 0;
		double				  distanceNm  = 1.;
		std::array<double, 3> boxDimensions { 90., 90., 90. };
		std::array<double, 3> boxAngles { 90., 90., 90. };

		double saltConcentration = 0.15;
	};
	// Hold all MDprep data specific to gromacs
	struct MdSettings
	{
		SystemSettings system;
		std::string	   protonationScript;
		MdStep		   nvt;
		MdStep		   npt;
		MdStep		   prod { .nstxout_compressed = 100 };
	};

	void boxShapeCollection( std::vector<std::string> & ) noexcept;
	void boxDimensionModeCollection( std::vector<std::string> & ) noexcept;
	void waterModelCollection( std::vector<std::string> & ) noexcept;

	void get( EngineSpecificCommonInformation & ) noexcept;

	class JobManager
	{
	  public:
		JobManager() = delete;
		JobManager( MdSettings & );

		void checkInputs( const MdParameters &, CheckReportCallback ) noexcept;
		void startPreparation( const MdParameters &, JobUpdateCallback ) noexcept;

		bool		isResultAvailable() const noexcept;
		CheckReport lastResult() const noexcept;

	  private:
		std::thread				 _thr;
		std::stack<std::jthread> _threadStack;
		CheckReport				 _report;
		MdSettings *			 _data;
		VTX::Util::SentryTarget	 _sentryTarget;
	};

} // namespace VTX::Tool::Mdprep::Gateway::Gromacs
#endif
