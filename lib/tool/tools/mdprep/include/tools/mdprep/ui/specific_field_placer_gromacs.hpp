#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__

class QFormLayout;
class QLineEdit;

namespace VTX::Tool::Mdprep::Gateway
{
	struct GromacsStep;
	struct GromacsSystem;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	class GromacsMdStepFieldPlacer
	{
	  public:
		GromacsMdStepFieldPlacer( Gateway::GromacsStep & );
		GromacsMdStepFieldPlacer() = delete;

		void placeFields( QFormLayout * ) noexcept;
		void apply() noexcept;

	  private:
		Gateway::GromacsStep * _data					 = nullptr;
		QLineEdit *			   _field_nstvout			 = nullptr;
		QLineEdit *			   _field_nstenergy			 = nullptr;
		QLineEdit *			   _field_nstlog			 = nullptr;
		QLineEdit *			   _field_nstxout_compressed = nullptr;
	};
	class GromacsSystemFieldPlacer
	{
	  public:
		GromacsSystemFieldPlacer( Gateway::GromacsSystem & );
		GromacsSystemFieldPlacer() = delete;

		void placeFields( QFormLayout * ) noexcept;
		void apply() noexcept;

	  private:
		Gateway::GromacsSystem * _data = nullptr;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
