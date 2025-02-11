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
#include <ui/qt/application.hpp>
#include <ui/qt/core/base_dock_widget.hpp>
#include <ui/qt/dock_widget/inspector.hpp>
#include <ui/qt/util.hpp>
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

	// Class responsible for managing the mdprep main window by coordinating the common form and the md engine
	// specifics.
	class MainWindow : public UI::QT::Core::BaseDockWidget<MainWindow>
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
		MainWindow( QWidget * const p_parent ) : UI::QT::Core::BaseDockWidget<MainWindow>( p_parent )
		{
			this->setWindowIcon( QIcon( ":/sprite/icon_tool_mdprep_mainButton.png" ) );
			this->setWindowTitle( "Molecular Dynamics Preparation" );

			_layout->addWidget( new QToolBar );

			QWidget * screenWidget = new QWidget;
			_layout->addWidget( screenWidget );

			setWindowState( Qt::WindowState::WindowActive );
			resize( PREFERRED_SIZE );
			_screen.emplace(
				screenWidget,
				_paramaeters,
				VTX::Tool::Mdprep::ui::ValidationSignaler { [ & ]( VTX::Tool::Mdprep::Gateway::JobUpdateIntermediate p_
															) { this->_preparationStarted( std::move( p_ ) ); } }
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
		g_win = APP_QT::getMainWindow()->createDockWidget<MainWindow>( Qt::RightDockWidgetArea );
		p_out = g_win;
	}

	struct OpenMdPrep : public App::UI::DescAction
	{
		OpenMdPrep()
		{
			name	 = "MdPrep";
			tip		 = "Prepare Molecular Dynamic Simulation";
			icon	 = "sprite/icon_tool_mdprep_mainButton.png";
			shortcut = "ctrl+alt+M";
			trigger	 = []()
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
			};
		}
	};

	void MdPrep::init() {}
	void MdPrep::onAppStart() {}
	void MdPrep::createUI()
	{
		OpenMdPrep action;
		APP_QT::addMenuAction( "Tool", action );
		APP_QT::addToolBarAction( "Tool", action );
	}
	void MdPrep::onAppStop() {}
} // namespace VTX::Tool::Mdprep
