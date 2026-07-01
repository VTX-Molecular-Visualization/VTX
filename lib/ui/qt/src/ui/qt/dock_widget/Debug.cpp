#include "ui/qt/dock_widget/debug.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include "app/ecs.hpp"
#include <QGroupBox>
#include <QPushButton>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/action/selection.hpp>

namespace VTX::UI::QT::DockWidget
{
	Debug::Debug( QWidget * p_parent ) : BaseDockWidget( p_parent, "Debug" )
	{
		using namespace Widget;
		using namespace Action;

		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::BUG ) );

		auto * groupBoxExtendSelection = new QGroupBox( "Extend selection" );
		auto * layoutExtendSelection   = new QHBoxLayout( groupBoxExtendSelection );

		// Bouton ExtendSelection
		auto * buttonExtendSelection = new QPushButton( "ExtendSelec", this );

		buttonExtendSelection->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );

		connect(
			buttonExtendSelection,
			&QPushButton::clicked,
			[ this ]() { App::ACTION().execute<App::Action::Selection::ExtendSelection>( 4.0 ); }
		);

		// Bouton ExtendSelection to residues
		auto * buttonExtendSelectionRes = new QPushButton( "ExtendSelecRes", this );

		buttonExtendSelectionRes->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );

		connect(
			buttonExtendSelectionRes,
			&QPushButton::clicked,
			[ this ]() { App::ACTION().execute<App::Action::Selection::ExtendSelectionRes>(); }
		);

		// Bouton RevertSelection
		auto * buttonRevertSelection = new QPushButton( "RevetSelection", this );

		buttonRevertSelection->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );

		connect(
			buttonRevertSelection,
			&QPushButton::clicked,
			[ this ]() { App::ACTION().execute<App::Action::Selection::RevertSelection>(); }
		);

		layoutExtendSelection->addWidget( buttonExtendSelection );
		layoutExtendSelection->addWidget( buttonExtendSelectionRes );
		layoutExtendSelection->addWidget( buttonRevertSelection );
		_layout->addWidget( groupBoxExtendSelection );
	}

	Debug::~Debug() {}
} // namespace VTX::UI::QT::DockWidget