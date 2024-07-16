#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS__

namespace VTX::Tool::Mdprep::Gateway
{
	struct GromacsData;
}
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

	  private:
		Gateway::GromacsData _data;
	};
	void get( const Gromacs &, Gateway::EngineSpecificCommonInformation & );
} // namespace VTX::Tool::Mdprep::ui

#endif
