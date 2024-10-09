#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_SHARED__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_SHARED__

class QLineEdit;
class QComboBox;
class QLabel;
class QFormLayout;
class QVBoxLayout;
class QString;
class QWidget;
class QDoubleSpinBox;

namespace VTX::Tool::Mdprep::ui::form_advanced
{

	struct UiData
	{
		QWidget * container = nullptr;

		QFormLayout *	 _layoutMinimization			   = nullptr;
		QComboBox *		 _fieldSystemBoxShape			   = nullptr;
		QComboBox *		 _fieldSystemWaterModel			   = nullptr;
		QComboBox *		 _fieldSystemBioForceField		   = nullptr;
		QComboBox *		 _fieldSystemChemForceField		   = nullptr;
		QLineEdit *		 _fieldMinimizationStepNumber	   = nullptr;
		QDoubleSpinBox * _fieldMinimizationStepSize		   = nullptr;
		QDoubleSpinBox * _fieldMinimizationStopThreshold   = nullptr;
		QLineEdit *		 _fieldProductionNstxoutCompressed = nullptr;
		struct FieldSet
		{
			QFormLayout *	 layout		  = nullptr;
			QLineEdit *		 nsteps		  = nullptr;
			QDoubleSpinBox * dt			  = nullptr;
			QLineEdit *		 saveInterval = nullptr;
		};
		FieldSet _equilibrationNvt;
		FieldSet _equilibrationNpt;
		FieldSet _production;

		QFormLayout * _layoutSystemPreparation = nullptr;
		QVBoxLayout * _systemReportLayout	   = nullptr;
		QPushButton * _buttonOpenExpertOptions = nullptr;
		QPushButton * _buttonStartInputCheck   = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui::form_advanced

#endif
