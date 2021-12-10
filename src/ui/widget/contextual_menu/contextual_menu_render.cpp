#include "contextual_menu_render.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "ui/dialog.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuRender::ContextualMenuRender( QWidget * const p_parent ) : BaseContextualMenu( p_parent ) {}
	ContextualMenuRender ::~ContextualMenuRender() {}

	void ContextualMenuRender::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuRender::_setupSlots()
	{
		addAction( "Load Molecule", this, &ContextualMenuRender::_loadMoleculeAction );
		addAction( "Download Molecule", this, &ContextualMenuRender::_downloadMoleculeAction );
		addSeparator();

		addAction( "Show All", this, &ContextualMenuRender::_showAllMoleculesAction );

		addSeparator();

		QMenu * const selectionGranularityMenu = new QMenu( this );
		selectionGranularityMenu->setTitle( "Selection target" );
		selectionGranularityMenu->addAction( "Atom" )->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME,
																	int( VTX::Selection::Granularity::ATOM ) );
		selectionGranularityMenu->addAction( "Residue" )
			->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME, int( VTX::Selection::Granularity::RESIDUE ) );
		selectionGranularityMenu->addAction( "Chain" )->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME,
																	 int( VTX::Selection::Granularity::CHAIN ) );
		selectionGranularityMenu->addAction( "Molecule" )
			->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME, int( VTX::Selection::Granularity::MOLECULE ) );
		connect(
			selectionGranularityMenu, &QMenu::triggered, this, &ContextualMenuRender::_setSelectionGranularityAction );

		addMenu( selectionGranularityMenu );
	}

	void ContextualMenuRender::localize() {}

	void ContextualMenuRender::_loadMoleculeAction() const { UI::Dialog::openLoadMoleculeDialog(); }
	void ContextualMenuRender::_downloadMoleculeAction() const { UI::Dialog::openDownloadMoleculeDialog(); }
	void ContextualMenuRender::_showAllMoleculesAction() const { VTX_ACTION( new Action::Main::ShowAllMolecules() ); }

	void ContextualMenuRender::_setSelectionGranularityAction( QAction * p_action ) const
	{
		const VTX::Selection::Granularity granularity
			= VTX::Selection::Granularity( p_action->property( SELECTION_GRANULARITY_PROPERTY_NAME ).toInt() );
		VTX_ACTION( new Action::Setting::ChangeSelectionGranularity( granularity ) );
	}

} // namespace VTX::UI::Widget::ContextualMenu
