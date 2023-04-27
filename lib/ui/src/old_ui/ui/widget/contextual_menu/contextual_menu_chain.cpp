#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_chain.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/qt/action/chain.hpp"

#include <app/action/chain.hpp>
#include <app/action/visible.hpp>
#include <app/model/representation/representation.hpp>
#include <app/model/representation/representation_library.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuChain::ContextualMenuChain( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuChain ::~ContextualMenuChain() {}

	void ContextualMenuChain::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_representationMenu
			= WidgetFactory::get().instantiateWidget<CustomWidget::SetRepresentationMenu>( this, "RepresentationMenu" );
	}
	void ContextualMenuChain::_setupSlots()
	{
		addMenu( _representationMenu );
		addSeparator();
		addAction( "Orient", this, &ContextualMenuChain::_orientAction );
		addAction( "Show", this, &ContextualMenuChain::_showAction );
		addAction( "Hide", this, &ContextualMenuChain::_hideAction );
		addAction( "Solo", this, &ContextualMenuChain::_soloAction );
		addAction( "Duplicate", this, &ContextualMenuChain::_copyAction );
		addAction( "Extract", this, &ContextualMenuChain::_extractAction );
		addAction( "Delete", this, &ContextualMenuChain::_deleteAction );

		connect( _representationMenu,
				 &CustomWidget::SetRepresentationMenu ::onRepresentationChange,
				 this,
				 &ContextualMenuChain::_applyRepresentationAction );
	}

	void ContextualMenuChain::localize() {}

	void ContextualMenuChain::setTarget( Model::Chain * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );

		const std::string & title = p_target->getName();
		setTitle( QString::fromStdString( title ) );
	}

	void ContextualMenuChain::_orientAction() { VTX_ACTION( new QT::Action::Chain::Orient( *_target ) ); }
	void ContextualMenuChain::_showAction()
	{
		VTX_ACTION(
			new App::Action::Chain::ChangeVisibility( *_target, App::Action::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuChain::_hideAction()
	{
		VTX_ACTION(
			new App::Action::Chain::ChangeVisibility( *_target, App::Action::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuChain::_soloAction()
	{
		VTX_ACTION(
			new App::Action::Chain::ChangeVisibility( *_target, App::Action::VISIBILITY_MODE::SOLO ) );
	}
	void ContextualMenuChain::_copyAction() { VTX_ACTION( new App::Action::Chain::Copy( *_target ) ); }
	void ContextualMenuChain::_extractAction() { VTX_ACTION( new App::Action::Chain::Extract( *_target ) ); }

	void ContextualMenuChain::_deleteAction() { VTX_ACTION( new App::Action::Chain::Delete( *_target ) ); }

	void ContextualMenuChain::_applyRepresentationAction( const int p_representationIndex )
	{
		VTX_ACTION( new App::Action::Chain::ChangeRepresentationPreset( *_target, p_representationIndex ) );
	}

} // namespace VTX::UI::Widget::ContextualMenu
