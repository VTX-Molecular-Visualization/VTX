#ifndef VTX_TOOL_MDPREP_UI_FORM_ADVANCED_EVENT_MANAGER_HPP
#define VTX_TOOL_MDPREP_UI_FORM_ADVANCED_EVENT_MANAGER_HPP

#include <array>

class QLineEdit;
class QComboBox;
class QLabel;
class QFormLayout;
class QPushButton;
class QString;
class QVBoxLayout;
class QWidget;
class QDoubleSpinBox;

namespace VTX::Util
{
	class SentryTarget;
}

namespace VTX::UI::QT::Util
{
	class LabelWithHelper;
	class ObjectOwnership;
} // namespace VTX::UI::QT::Util

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	struct MdAdvancedDataSample;
	struct MdBasicDataSample;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	class MdEngineSpecificFieldPlacer;
	class InputChecker;
	class ReportManager;
} // namespace VTX::Tool::Mdprep::ui

namespace VTX::Tool::Mdprep::ui::form_advanced
{
	class EventManager
	{
	  public:
		std::array<MdEngineSpecificFieldPlacer, 5> fieldPlacers;
		ReportManager *							   reportManager;
		Gateway::MdParameters					   parameters;

		EventManager() = delete;
		EventManager( ReportManager &, Gateway::MdParameters );
		~EventManager();
		EventManager( EventManager && ) noexcept;
		EventManager & operator=( EventManager && ) noexcept;
		EventManager( const EventManager & )			 = delete;
		EventManager & operator=( const EventManager & ) = delete;

		void connectBoxShape( QComboBox * p_fieldSystemBoxShape );
		void connectInputCheck( QPushButton * p_buttonStartInputCheck );
		void connectBioForceField( QComboBox * p_fieldBioForceField, QVBoxLayout * p_target );
		void performFirstInputCheck( const Gateway::MdParameters & ) const;

	  private:
		QComboBox *				_fieldSystemBoxShape	   = nullptr;
		QComboBox *				_fieldBioForceField		   = nullptr;
		QVBoxLayout *			_bioForcefieldResultLayout = nullptr;
		QPushButton *			_buttonStartInputCheck	   = nullptr;
		VTX::Util::SentryTarget _sentry;
		void					_connectAll();
		void					_connectBoxShape();
		void					_connectInputCheck();
		void					_connectForceField();
		std::function<void()>	_getInputCheckCallback();
	};
} // namespace VTX::Tool::Mdprep::ui::form_advanced

#endif
