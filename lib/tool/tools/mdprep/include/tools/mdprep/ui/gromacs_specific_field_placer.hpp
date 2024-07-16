#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SPECIFIC_FIELD_PLACER_GROMACS__

class QFormLayout;
class QLineEdit;
class QCombobox;
class QDoubleSpinBox;

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
		Gateway::GromacsSystem * _data			   = nullptr;
		QCombobox *				 _fieldBoxSizeMode = nullptr;
		QDoubleSpinBox *		 _fieldDistance	   = nullptr;
		QLineEdit *				 _fieldSizeX	   = nullptr;
		QLineEdit *				 _fieldSizeY	   = nullptr;
		QLineEdit *				 _fieldSizeZ	   = nullptr;
		QLineEdit *				 _fieldAngleX	   = nullptr;
		QLineEdit *				 _fieldAngleY	   = nullptr;
		QLineEdit *				 _fieldAngleZ	   = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
