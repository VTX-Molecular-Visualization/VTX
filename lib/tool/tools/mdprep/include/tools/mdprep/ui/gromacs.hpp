#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS__

namespace VTX::Tool::Mdprep::Gateway::Gromacs
{
	struct MdSettings;
}
namespace VTX::Tool::Mdprep::Gateway
{
	class EngineJobManager;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	class MdEngineFieldPlacer;
	class MdEngineSpecificFieldPlacer;

	// Class responsible for providing gromacs specific ui objects
	class Gromacs
	{
	  public:
		void get( MdEngineFieldPlacer & p_out ) noexcept;
		void get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & ) noexcept;
		void get( Gateway::EngineJobManager & ) noexcept;

	  private:
		Gateway::Gromacs::MdSettings _data;
	};
	void get( const Gromacs &, Gateway::EngineSpecificCommonInformation & ) noexcept;
} // namespace VTX::Tool::Mdprep::ui

#endif
