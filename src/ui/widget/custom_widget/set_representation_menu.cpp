#include "set_representation_menu.hpp"
#include "action/action_manager.hpp"
#include "action/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/widget/custom_widget/indexed_action.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	void SetRepresentationMenu::_applyRepresentation( const int p_representationIndex )
	{
		emit onRepresentationChange( p_representationIndex );
	}

	void SetRepresentationMenu::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setTitle( "Representation" );

		_representationGroup = new QActionGroup( this );
		_representationGroup->setExclusive( true );

		for ( int i = 0; i < Model::Representation::RepresentationLibrary::get().getRepresentationCount(); i++ )
		{
			QAction * const setRepresentationAction = _generateSetRepresentationAction( i );
			_representationGroup->addAction( setRepresentationAction );
			addAction( setRepresentationAction );
		}
	}
	void SetRepresentationMenu::_setupSlots() {}

	QAction * SetRepresentationMenu::_generateSetRepresentationAction( const int p_representationIndex )
	{
		const Model::Representation::BaseRepresentation * const representation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( p_representationIndex );

		CustomWidget::IndexedAction * const res = new CustomWidget::IndexedAction( p_representationIndex, this );
		res->setText( QString::fromStdString( representation->getName() ) );
		res->setCheckable( true );

		connect(
			res, &CustomWidget::IndexedAction::triggeredWithIndex, this, &SetRepresentationMenu::_applyRepresentation );

		return res;
	}

	void SetRepresentationMenu::tickCurrentRepresentation( const int p_representationIndex ) const
	{
		QAction * const tickedAction = actions()[ p_representationIndex ];
		tickedAction->setChecked( true );
	}
	void SetRepresentationMenu::removeTick() const
	{
		if ( _representationGroup->checkedAction() != nullptr )
			_representationGroup->checkedAction()->setChecked( false );
	}

	void SetRepresentationMenu::localize() {}

} // namespace VTX::UI::Widget::CustomWidget
