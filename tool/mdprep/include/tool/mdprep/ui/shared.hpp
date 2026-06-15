#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SHARED__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SHARED__

#include <tool/mdprep/gateway/shared.hpp>
#include <ui/qt/util.hpp>

class QVBoxLayout;
class QFormLayout;
class QWidget;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;

namespace VTX::Tool::Mdprep::Gateway
{
	struct SystemData;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{

	// Datastructure allowing engine specific fields to be placed with regard for the form
	struct FormLayouts
	{
		QFormLayout * baseParametersIon					 = nullptr;
		QFormLayout * advancedParametersSystemPrepartion = nullptr;
		QFormLayout * advancedParametersMinimization	 = nullptr;
		QFormLayout * advancedParametersEquilibrationNvt = nullptr;
		QFormLayout * advancedParametersEquilibrationNpt = nullptr;
		QFormLayout * advancedParametersProduction		 = nullptr;
	};

	// Fields are grouped into the following sections
	enum class E_FIELD_SECTION
	{
		system,
		minimization,
		equilibrationNvt,
		equilibrationNpt,
		production,
		ions
	};

	enum class E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD
	{
		bioForcefield,
		boxShape,
		boxDimensionMode,
		waterModel,
		COUNT
	};

	struct EngineSpecificCommonInformationFieldUpdate
	{
		E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD field	= E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD::bioForcefield;
		size_t									   newValue = 0xffffffffffffffff;
	};

	enum class E_COMMON_FIELDS_COMBOBOX
	{
		bioForcefield,
		chemForcefield,
		boxShape,
		waterModel,
		COUNT
	};
	void get( const E_COMMON_FIELDS_COMBOBOX &, QWidget *& label, QComboBox *& field ) noexcept;

	enum class E_COMMON_FIELDS_DOUBLE
	{
		minimizationStepSize,
		minimizationEnergyBreakpoint,
		dt,
		COUNT
	};
	void get( const E_COMMON_FIELDS_DOUBLE &, QWidget *& label, QDoubleSpinBox *& field ) noexcept;

	enum class E_COMMON_FIELDS_UINT64
	{
		stepNumberMinimization,
		stepNumber,
		coordinateSaveInterval,
		COUNT
	};
	void get( const E_COMMON_FIELDS_UINT64 &, QWidget *& label, QLineEdit *& field ) noexcept;

	// class responsible for putting on fields and gathering data.
	// Fields are destroyed when the object is destroyed.
	class IonicFields
	{
	  public:
		IonicFields() = default;
		IonicFields( QVBoxLayout * p_target, const Gateway::SystemData & p_defaults );

		void get( Gateway::SystemData & p_out ) const noexcept;

	  private:
		VTX::UI::QT::Util::ObjectOwnership _toDelete;

		struct UiObjects
		{
			QWidget *	container	 = nullptr;
			QComboBox * cationCharge = nullptr;
			QComboBox * anionCharge	 = nullptr;
			QLineEdit * anionName	 = nullptr;
			QLineEdit * cationName	 = nullptr;
		} _uiObjects;

		// test if this instance is a default-constructed one
		bool _isDefault() const noexcept;
		void _setupUi( QVBoxLayout * p_target ) noexcept;
		void _loadValues( const Gateway::SystemData & p_defaults ) noexcept;
	};

	inline const char * getReportLabel( const Gateway::CheckReport & p_report ) noexcept
	{
		if ( p_report.dirty )
		{
			return "System needs check.";
		}
		else if ( p_report.pass )
		{
			return "System compatible with MD Engine";
		}
		return "An incompatibility was found between the system and the MD Engine";
	}

} // namespace VTX::Tool::Mdprep::ui
#endif
