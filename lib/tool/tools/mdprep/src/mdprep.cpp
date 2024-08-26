#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextdocument.h>
//
//
#include "tools/mdprep/gateway/engine_job_manager.hpp"
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/input_checker.hpp"
#include "tools/mdprep/ui/shared.hpp"
#include "util/sentry.hpp"
//
#include "tools/mdprep/ui/report.hpp"
//
#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/ui/md_engine.hpp"
#include "tools/mdprep/ui/md_engine_factory.hpp"
#include "tools/mdprep/ui/md_engine_field_placer.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/form_switch_button.hpp"
#include <ui/qt/application_qt.hpp>
#include <ui/qt/main_window.hpp>
#include <ui/qt/util.hpp>
#include <ui/qt/widget_factory.hpp>
#include <util/logger.hpp>
//
#include "tools/mdprep/ui/form_advanced/event_manager.hpp"
#include "tools/mdprep/ui/form_advanced/form_advanced.hpp"
#include "tools/mdprep/ui/form_basic/form_basic.hpp"
#include "tools/mdprep/ui/screen_forms.hpp"
//
#include "tools/mdprep/ui/form.hpp"
//

namespace VTX::QT::Mdprep
{

	// Class responsible for managing the mdprep main window by coordinating the common form and the md engine
	// specifics.
	class MainWindow : public UI::QT::QtDockablePanel
	{
		inline static const QSize PREFERRED_SIZE { 500, 720 };

		using EngineCollection
			= std::array<std::optional<VTX::Tool::Mdprep::ui::MdEngine>, VTX::Tool::Mdprep::ui::MD_ENGINE_NUMBER>;
		QComboBox * _w_mdEngine	   = nullptr;
		QWidget *	_formContainer = nullptr;

		VTX::Tool::Mdprep::Gateway::MdParameters		  _paramaeters;
		std::optional<VTX::Tool::Mdprep::ui::ScreenForms> _screen;

		virtual void _setupUi( const QString & p_name )
		{
			auto		_t = p_name.toLatin1();
			std::string v( _t.begin(), _t.end() );
			QWidget *	mainWidget = _instantiateMainWidget( PREFERRED_SIZE, PREFERRED_SIZE );

			UI::QT::QtDockablePanel::_setupUi( p_name );
			this->setWindowIcon( QIcon( ":/sprite/icon_tool_mdprep_mainButton.png" ) );
			this->setWindowTitle( "Molecular Dynamics Preparation" );

			setWindowState( Qt::WindowState::WindowActive );
			const QSize winsize = PREFERRED_SIZE;
			resize( winsize );

			_screen.emplace( mainWidget, _paramaeters, VTX::Tool::Mdprep::ui::ValidationSignaler {} );
		}

	  public:
		MainWindow( QWidget * const p_parent ) : UI::QT::QtDockablePanel( p_parent ) {}
	};
} // namespace VTX::QT::Mdprep

namespace VTX::Tool::Mdprep
{

	class MainWindow::_impl
	{
		VTX::QT::Mdprep::MainWindow * _win
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::QT::Mdprep::MainWindow>(
				reinterpret_cast<QWidget *>( &VTX::UI::QT::QT_APP()->getMainWindow() ),
				"MdPrep Tool"
			);

	  public:
		_impl() {}
		void show() noexcept { _win->show(); }
	};
	MainWindow::MainWindow() : _pimpl( new MainWindow::_impl() ) {}

	// Assumes pimpl is always valid ptr
	void MainWindow::show() noexcept { _pimpl->show(); }
	void MainWindow::Del::operator()( _impl * p_ ) noexcept { delete p_; }
} // namespace VTX::Tool::Mdprep
