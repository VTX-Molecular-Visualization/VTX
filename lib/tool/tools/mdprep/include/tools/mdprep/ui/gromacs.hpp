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
	void get( const Gromacs &, Gateway::EngineSpecificCommonInformation & ) noexcept;

	struct BoxFields
	{
		QCombobox *		 _fieldBoxSizeMode = nullptr;
		QDoubleSpinBox * _fieldDistance	   = nullptr;
		QLineEdit *		 _fieldSizeX	   = nullptr;
		QLineEdit *		 _fieldSizeY	   = nullptr;
		QLineEdit *		 _fieldSizeZ	   = nullptr;
		QLineEdit *		 _fieldAngleX	   = nullptr;
		QLineEdit *		 _fieldAngleY	   = nullptr;
		QLineEdit *		 _fieldAngleZ	   = nullptr;
	};

	// Make all of it a class so box GromacsMdStepFieldPlacer and MdAdvancedParamForm can make use of it
	//
	//  Create and place fields into the form
	void createFields( QFormLayout *, BoxFields & ) noexcept;

	void updateBoxShape( QFormLayout *, BoxFields & ) noexcept;
	void updateBoxSizeMode( QFormLayout *, BoxFields & ) noexcept;

} // namespace VTX::Tool::Mdprep::ui

#endif
