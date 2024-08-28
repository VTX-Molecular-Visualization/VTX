#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextdocument.h>
//
//
#include "tool/mdprep/gateway/engine_job_manager.hpp"
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/shared.hpp"
#include "util/sentry.hpp"
//
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/mdprep.hpp"
#include "tool/mdprep/ui/md_engine.hpp"
#include "tool/mdprep/ui/md_engine_factory.hpp"
#include "tool/mdprep/ui/md_engine_field_placer.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tool/mdprep/ui/form_switch_button.hpp"
#include <qt/application_qt.hpp>
#include <qt/main_window.hpp>
#include <qt/util.hpp>
#include <qt/widget_factory.hpp>
#include <util/logger.hpp>
//
#include "tool/mdprep/ui/form.hpp"
#include "tool/mdprep/ui/form_advanced/event_manager.hpp"
#include "tool/mdprep/ui/form_advanced/form_advanced.hpp"
#include "tool/mdprep/ui/form_basic/form_basic.hpp"
#include "tool/mdprep/ui/screen_forms.hpp"
//
#include "tool/mdprep/ui/form.hpp"
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
		std::optional<VTX::Tool::Mdprep::Gateway::JobUpdateIntermediate>
			__tmp; // Once the job progress view screen is done, it should be removed

		virtual void _setupUi( const QString & p_name )
		{
			auto		_t = p_name.toLatin1();
			std::string v( _t.begin(), _t.end() );
			QWidget *	mainWidget = _instantiateMainWidget( PREFERRED_SIZE, PREFERRED_SIZE );
			mainWidget->setContentsMargins( { 0, 0, 0, 0 } );

			UI::QT::QtDockablePanel::_setupUi( p_name );
			this->setWindowIcon( QIcon( ":/sprite/icon_tool_mdprep_mainButton.png" ) );
			this->setWindowTitle( "Molecular Dynamics Preparation" );

			setWindowState( Qt::WindowState::WindowActive );
			const QSize winsize = PREFERRED_SIZE;
			resize( winsize );

			_screen.emplace(
				mainWidget,
				_paramaeters,
				VTX::Tool::Mdprep::ui::ValidationSignaler { [ & ]( VTX::Tool::Mdprep::Gateway::JobUpdateIntermediate p_
															) { this->_preparationStarted( std::move( p_ ) ); } }
			);
		}
		virtual void _setupSlots() override {}
		void		 _preparationStarted( VTX::Tool::Mdprep::Gateway::JobUpdateIntermediate p_ )
		{
			__tmp.emplace( std::move( p_ ) ); // TMP
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
