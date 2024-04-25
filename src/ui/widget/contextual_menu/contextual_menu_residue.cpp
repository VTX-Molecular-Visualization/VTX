#include "contextual_menu_residue.hpp"
#include "action/action_manager.hpp"
#include "action/residue.hpp"
#include "action/visible.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/dialog.hpp"
#include "ui/widget_factory.hpp"
#include "util/solvent_excluded_surface.hpp"

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

	void ContextualMenuResidue::setTarget( Model::Residue * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );

		const std::string & title = p_target->getDefaultName();
		setTitle( QString::fromStdString( title ) );
	}

	void ContextualMenuResidue::_orientAction() { VTX_ACTION( new Action::Residue::Orient( *_target ) ); }
	void ContextualMenuResidue::_showAction()
	{
		VTX_ACTION( new Action::Residue::ChangeVisibility( *_target,
														   Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuResidue::_hideAction()
	{
		VTX_ACTION( new Action::Residue::ChangeVisibility( *_target,
														   Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuResidue::_soloAction()
	{
		VTX_ACTION( new Action::Residue::ChangeVisibility( *_target,
														   Action::Visible::ChangeVisibility::VISIBILITY_MODE::SOLO ) );
	}
	void ContextualMenuResidue::_copyAction() { VTX_ACTION( new Action::Residue::Copy( *_target ) ); }
	void ContextualMenuResidue::_extractAction() { VTX_ACTION( new Action::Residue::Extract( *_target ) ); }
	void ContextualMenuResidue::_deleteAction() { VTX_ACTION( new Action::Residue::Delete( *_target ) ); }

	void ContextualMenuResidue::_applyRepresentationAction( const int p_representationIndex )
	{
		const bool isTryingToApplySES = Model::Representation::RepresentationLibrary::get()
											.getRepresentation( p_representationIndex )
											->getRepresentationType()
										== Generic::REPRESENTATION::SES;

		if ( isTryingToApplySES )
		{
			Model::Category *	category = _target->getMoleculePtr()->getCategoryFromChain( *_target->getChainPtr() );
			const CATEGORY_ENUM categoryEnum = category->getCategoryEnum();
			if ( categoryEnum == CATEGORY_ENUM::POLYMER || categoryEnum == CATEGORY_ENUM::CARBOHYDRATE )
			{
				if ( category->getMolecule()->hasSolventExcludedSurface( categoryEnum ) == false )
				{
					auto [ isBig, memory ] = Util::SolventExcludedSurface::checkSESMemory( *category );
					if ( isBig && Dialog::bigSESComputationWarning( memory ) == false )
					{
						return;
					}
				}
			}
		}

		VTX_ACTION( new Action::Residue::ChangeRepresentationPreset( *_target, p_representationIndex ) );
	}

} // namespace VTX::UI::Widget::ContextualMenu
