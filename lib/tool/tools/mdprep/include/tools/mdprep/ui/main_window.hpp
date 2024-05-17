#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MAINWINDOW__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MAINWINDOW__

#include <array>
#include <string>

namespace VTX::Tool::Mdprep::ui
{
	enum class E_MD_ENGINE
	{
		gromacs,
		anotherone,
		COUNT
	};
	constexpr const size_t MD_ENGINE_NUMBER = static_cast<size_t>( E_MD_ENGINE::COUNT );
	constexpr const char * string( const E_MD_ENGINE & ) noexcept;

	const std::array<const char *, MD_ENGINE_NUMBER> & mdEngineStrings();

	class MdEngineForm;
	struct FormLayouts;
	MdEngineForm form( const E_MD_ENGINE &, FormLayouts ) noexcept;

} // namespace VTX::Tool::Mdprep::ui

#endif
