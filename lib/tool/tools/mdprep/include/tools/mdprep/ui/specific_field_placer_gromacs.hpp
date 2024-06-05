#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__

class QFormLayout;
class QLineEdit;

namespace VTX::Tool::Mdprep::Gateway
{
	struct GromacsStep;
}

namespace VTX::Tool::Mdprep::ui
{
	class SpecificFieldPlacerGromacs
	{
	  public:
		SpecificFieldPlacerGromacs( Gateway::GromacsStep & );
		SpecificFieldPlacerGromacs() = delete;

		void placeFields( QFormLayout * ) noexcept;
		void apply() noexcept;

	  private:
		Gateway::GromacsStep * _data					 = nullptr;
		QLineEdit *			   _field_nstvout			 = nullptr;
		QLineEdit *			   _field_nstenergy			 = nullptr;
		QLineEdit *			   _field_nstlog			 = nullptr;
		QLineEdit *			   _field_nstxout_compressed = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
