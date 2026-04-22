#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextDocument>
//
#include <qtoolbar.h>
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
#include <QDockWidget>
#include <app/tool/base_tool.hpp>
#include <ui/qt/action_registry.hpp>
#include <ui/qt/application.hpp>
#include <ui/qt/dock_widget/base_dock_widget.hpp>
#include <ui/qt/services.hpp>
#include <ui/qt/util.hpp>
#include <ui/qt/widget/main_window.hpp>
#include <string_view>
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

namespace VTX::Tool::Mdprep
{
	namespace
	{
		constexpr std::string_view ACTION_OPEN_MDPREP = "tool.mdprep.open";
	}

	// Class responsible for managing the mdprep main window by coordinating the common form and the md engine
	// specifics.
	class MainWindow : public UI::QT::DockWidget::BaseDockWidget<MainWindow>
	{
	  public:
		inline static const QSize PREFERRED_SIZE { 500, 720 };

	  private:
		using EngineCollection
			= std::array<std::optional<VTX::Tool::Mdprep::ui::MdEngine>, VTX::Tool::Mdprep::ui::MD_ENGINE_NUMBER>;
		QComboBox * _w_mdEngine	   = nullptr;
		QWidget *	_formContainer = nullptr;

		VTX::Tool::Mdprep::Gateway::MdParameters		  _paramaeters;
		std::optional<VTX::Tool::Mdprep::ui::ScreenForms> _screen;
		std::optional<VTX::Tool::Mdprep::Gateway::JobUpdateIntermediate>
			__tmp; // Once the job progress view screen is done, it should be removed

		void _preparationStarted( VTX::Tool::Mdprep::Gateway::JobUpdateIntermediate p_ )
		{
			__tmp.emplace( std::move( p_ ) ); // TMP
		}

	  public:
		MainWindow( QWidget * const p_parent ) : BaseDockWidget( p_parent, "Molecular Dynamics Preparation" )
		{
			this->setWindowIcon( QIcon( ":/sprite/icon_tool_mdprep_mainButton.png" ) );

			_layout->addWidget( new QToolBar );

			QWidget * screenWidget = new QWidget;
			_layout->addWidget( screenWidget );

			setWindowState( Qt::WindowState::WindowActive );
			resize( PREFERRED_SIZE );
			_screen.emplace(
				screenWidget,
				_paramaeters,
				VTX::Tool::Mdprep::ui::ValidationSignaler {
					[ & ]( VTX::Tool::Mdprep::Gateway::JobUpdateIntermediate p_ )
					{ this->_preparationStarted( std::move( p_ ) ); } }
			);
		}
	};

	MainWindow * g_win = nullptr;
	void		 get( MainWindow *& p_out ) noexcept
	{
		if ( g_win )
		{
			p_out = g_win;
			return;
		}
		auto & mainWindow = UI::QT::MAIN_WINDOW();
		g_win			  = mainWindow.createDockWidget<MainWindow>( Qt::RightDockWidgetArea );
		p_out			  = g_win;
	}

	App::UI::DescAction openMdPrepAction()
	{
		App::UI::DescAction action;
		action.key		= ACTION_OPEN_MDPREP;
		action.name		= "MdPrep";
		action.tip		= "Prepare Molecular Dynamic Simulation";
		action.icon		= "sprite/icon_tool_mdprep_mainButton.png";
		action.shortcut = "ctrl+alt+M";
		return action;
	}

	void openMdPrep()
	{
		MainWindow * win;
		get( win );

		if ( win->isHidden() )
		{
			win->resize( win->PREFERRED_SIZE );
			win->show();
			win->raise();
		}
		else
			win->hide();
	}

	MdPrep::MdPrep() = default;

	void MdPrep::registerActions()
	{
		UI::QT::UI_ACTIONS().registerAction(
			openMdPrepAction(), []( const UI::QT::ActionRegistry::ActionContext & ) { openMdPrep(); }
		);
	}

	void MdPrep::buildUI()
	{
		auto & mainWindow = UI::QT::MAIN_WINDOW();
		mainWindow.addMenuAction( "Tool", ACTION_OPEN_MDPREP );
		mainWindow.addToolBarAction( "Tool", ACTION_OPEN_MDPREP );
	}

} // namespace VTX::Tool::Mdprep
