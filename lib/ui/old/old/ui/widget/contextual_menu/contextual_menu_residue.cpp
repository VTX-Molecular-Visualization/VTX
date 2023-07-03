#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_residue.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/qt/action/residue.hpp"
#include <app/old/action/residue.hpp>
#include <app/old/action/visible.hpp>
#include <app/old/application/representation/representation_library.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuResidue::ContextualMenuResidue( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuResidue ::~ContextualMenuResidue() {}

	void ContextualMenuResidue::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_representationMenu
			= WidgetFactory::get().instantiateWidget<CustomWidget::SetRepresentationMenu>( this, "RepresentationMenu" );
	}
	void ContextualMenuResidue::_setupSlots()
	{
		addMenu( _representationMenu );
		addSeparator();
		addAction( "Orient", this, &ContextualMenuResidue::_orientAction );
		addAction( "Show", this, &ContextualMenuResidue::_showAction );
		addAction( "Hide", this, &ContextualMenuResidue::_hideAction );
		addAction( "Solo", this, &ContextualMenuResidue::_soloAction );
		addAction( "Duplicate", this, &ContextualMenuResidue::_copyAction );
		addAction( "Extract", this, &ContextualMenuResidue::_extractAction );
		addAction( "Delete", this, &ContextualMenuResidue::_deleteAction );

		connect( _representationMenu,
				 &CustomWidget::SetRepresentationMenu ::onRepresentationChange,
				 this,
				 &ContextualMenuResidue::_applyRepresentationAction );
	}

	void ContextualMenuResidue::localize() {}

	void ContextualMenuResidue::setTarget( App::Old::Component::Chemistry::Residue * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );

		const std::string & title = p_target->getDefaultName();
		setTitle( QString::fromStdString( title ) );
	}

	void ContextualMenuResidue::_orientAction() { VTX_ACTION( new QT::Action::Residue::Orient( *_target ) ); }
	void ContextualMenuResidue::_showAction()
	{
		VTX_ACTION( new App::Old::Action::Residue::ChangeVisibility( *_target, App::Old::Action::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuResidue::_hideAction()
	{
		VTX_ACTION( new App::Old::Action::Residue::ChangeVisibility( *_target, App::Old::Action::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuResidue::_soloAction()
	{
		VTX_ACTION( new App::Old::Action::Residue::ChangeVisibility( *_target, App::Old::Action::VISIBILITY_MODE::SOLO ) );
	}
	void ContextualMenuResidue::_copyAction() { VTX_ACTION( new App::Old::Action::Residue::Copy( *_target ) ); }
	void ContextualMenuResidue::_extractAction() { VTX_ACTION( new App::Old::Action::Residue::Extract( *_target ) ); }
	void ContextualMenuResidue::_deleteAction() { VTX_ACTION( new App::Old::Action::Residue::Delete( *_target ) ); }

	void ContextualMenuResidue::_applyRepresentationAction( const int p_representationIndex )
	{
		VTX_ACTION( new App::Old::Action::Residue::ChangeRepresentationPreset( *_target, p_representationIndex ) );
	}

} // namespace VTX::UI::Widget::ContextualMenu
