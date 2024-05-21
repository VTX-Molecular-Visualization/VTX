#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MAINWINDOW__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MAINWINDOW__

#include <array>
#include <string>

class QWidget;

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

	// Class responsible for settings up the MD UI with respect of basic VS advanced MD parameters
	class MdFieldsOrganizer
	{
	  public:
		QWidget * basicParamContainer	 = nullptr;
		QWidget * advancedParamContainer = nullptr;

		// Construct only for data allocation
		MdFieldsOrganizer() = default;

		// Do the work. layouts won't be nullptr after that.
		void setupUi( QLayout * ) noexcept;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
