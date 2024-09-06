#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_EVENT_MANAGER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_EVENT_MANAGER__

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
	class UiReportManager;
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

		void connectBoxShape( QComboBox * p_fieldSystemBoxShape ) noexcept;
		void connectInputCheck( QPushButton * p_buttonStartInputCheck ) noexcept;
		void connectBioForceField( QComboBox * p_fieldBioForceField, QVBoxLayout * p_target ) noexcept;
		void performFirstInputCheck( const Gateway::MdParameters & ) noexcept;

	  private:
		QComboBox *				_fieldSystemBoxShape	   = nullptr;
		QComboBox *				_fieldBioForceField		   = nullptr;
		QVBoxLayout *			_bioForcefieldResultLayout = nullptr;
		QPushButton *			_buttonStartInputCheck	   = nullptr;
		UiReportManager			_uiReportManager;
		VTX::Util::SentryTarget _sentry;
		void					_connectAll() noexcept;
		void					_connectBoxShape() noexcept;
		void					_connectInputCheck() noexcept;
		void					_connectForceField() noexcept;
		std::function<void()>	_getInputCheckCallback() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui::form_advanced

#endif
