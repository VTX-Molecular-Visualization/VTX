#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__

class QFormLayout;
class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class QWidget;

namespace VTX::Tool::Mdprep::Gateway
{
	struct GromacsStep;
	struct GromacsSystem;
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	struct EngineSpecificCommonInformationFieldUpdate;
	class MdEngine;
	class GromacsMdStepFieldPlacer
	{
	  public:
		GromacsMdStepFieldPlacer( Gateway::GromacsStep & );
		GromacsMdStepFieldPlacer() = delete;

		void placeFields( QFormLayout * ) noexcept;
		void apply() noexcept;
		void get( const MdEngine *& p_out ) const noexcept;

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
		void update( const EngineSpecificCommonInformationFieldUpdate & ) noexcept;
		void get( const MdEngine *& p_out ) const noexcept;

	  private:
		Gateway::GromacsSystem * _data = nullptr;

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
		Gateway::E_EDITCONF_BOX_SHAPE _currentShape = Gateway::E_EDITCONF_BOX_SHAPE::dodecahedron;
		QFormLayout *				  _targetLayout = nullptr;
		BoxFields					  _boxFields;

		void _removeFields() noexcept;
		void _updateBoxSizeMode( ) noexcept;

		void _addDimensionModeField() noexcept;
		void _addDistanceField() noexcept;
		void _addSizeOnlyX() noexcept;
		void _addSizeXYZ() noexcept;
		void _addAnglesXYZ() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
