#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__

class QFormLayout;
class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class QWidget;

namespace VTX::Tool::Mdprep::Gateway::Gromacs
{
	struct MdStep;
	struct SystemSettings;
} // namespace VTX::Tool::Mdprep::Gateway::Gromacs

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	struct EngineSpecificCommonInformationFieldUpdate;
	class MdEngine;

	class GromacsMdStepFieldPlacer
	{
	  public:
		GromacsMdStepFieldPlacer( Gateway::Gromacs::MdStep & );
		GromacsMdStepFieldPlacer() = delete;

		void placeFields( QFormLayout * );
		void apply();

	  private:
		Gateway::Gromacs::MdStep * _data					 = nullptr;
		QLineEdit *				   _field_nstvout			 = nullptr;
		QLineEdit *				   _field_nstenergy			 = nullptr;
		QLineEdit *				   _field_nstlog			 = nullptr;
		QLineEdit *				   _field_nstxout_compressed = nullptr;
	};

	class GromacsSystemFieldPlacer
	{
	  public:
		GromacsSystemFieldPlacer( Gateway::Gromacs::SystemSettings & );
		GromacsSystemFieldPlacer() = delete;

		void placeFields( QFormLayout * );
		void apply();
		void update( const EngineSpecificCommonInformationFieldUpdate & );

	  private:
		Gateway::Gromacs::SystemSettings * _data = nullptr;

		struct BoxFields
		{
			QComboBox *		 fieldBoxSizeMode  = nullptr;
			QDoubleSpinBox * fieldDistance	   = nullptr;
			QWidget *		 containerSizeXYZ  = nullptr;
			QLineEdit *		 fieldSizeX		   = nullptr;
			QLineEdit *		 fieldSizeY		   = nullptr;
			QLineEdit *		 fieldSizeZ		   = nullptr;
			QWidget *		 containerAngleXYZ = nullptr;
			QLineEdit *		 fieldAngleX	   = nullptr;
			QLineEdit *		 fieldAngleY	   = nullptr;
			QLineEdit *		 fieldAngleZ	   = nullptr;
		};

		Gateway::Gromacs::E_BOX_SHAPE _currentShape = Gateway::Gromacs::E_BOX_SHAPE::dodecahedron;
		QFormLayout *				  _targetLayout = nullptr;
		BoxFields					  _boxFields;

		void _removeFields();
		void _updateBoxSizeMode();

		void _addDimensionModeField();
		void _addDistanceField();
		void _addSizeOnlyX();
		void _addSizeXYZ();
		void _addAnglesXYZ();
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
