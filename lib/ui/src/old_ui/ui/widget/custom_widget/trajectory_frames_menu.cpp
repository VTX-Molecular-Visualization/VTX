#include "ui/old_ui/ui/widget/custom_widget/trajectory_frames_menu.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/molecule.hpp>
#include <app/old_app/model/generated_molecule.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/mvc/mvc_manager.hpp>
#include <util/string.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	TrajectoryFramesMenu::TrajectoryFramesMenu( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

	void TrajectoryFramesMenu::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setTitle( "Frames" );

		_allFramesAction = new UIAction::SelfReferencedAction( "All", this );
		_allFramesAction->setData( QVariant( Model::GeneratedMolecule::ALL_FRAMES_SEPARATED_INDEX ) );
		connect( _allFramesAction,
				 &UIAction::SelfReferencedAction::triggeredSelf,
				 this,
				 &TrajectoryFramesMenu::_onFrameSelected );

		if ( _allFramesOption )
		{
			addAction( _allFramesAction );
		}
	}

	void TrajectoryFramesMenu::_setupSlots() {}
	void TrajectoryFramesMenu::localize() {}

	uint TrajectoryFramesMenu::getFrameCount() const { return _frameCount; }

	void TrajectoryFramesMenu::setFrameDisplayString( const std::string & p_str )
	{
		_frameDisplayNameStr = p_str;
		_updateFrameNames();
	}

	void TrajectoryFramesMenu::updateFrames( const Model::Molecule & p_molecule )
	{
		_adjustFrameActions( p_molecule.getFrameCount() );
	}
	void TrajectoryFramesMenu::updateFrames( const Model::Selection & p_selection )
	{
		uint maxNbFrame = 1;

		for ( const Model::Selection::PairMoleculeIds & pairMolecule : p_selection.getMoleculesMap() )
		{
			const Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( pairMolecule.first );
			maxNbFrame = maxNbFrame < molecule.getFrameCount() ? molecule.getFrameCount() : maxNbFrame;
		}

		_adjustFrameActions( maxNbFrame );
	}

	void TrajectoryFramesMenu::_adjustFrameActions( const uint p_newFrameCount )
	{
		if ( p_newFrameCount > _frameCount )
		{
			for ( uint i = _frameCount; i < p_newFrameCount; i++ )
			{
				const QString						   actionTitle = _getFrameDisplayName( i );
				UIAction::SelfReferencedAction * const action = new UIAction::SelfReferencedAction( actionTitle, this );
				action->setData( QVariant( int( i ) ) );

				connect( action,
						 &UIAction::SelfReferencedAction::triggeredSelf,
						 this,
						 &TrajectoryFramesMenu::_onFrameSelected );

				addAction( action );
			}

			_frameCount = p_newFrameCount;
		}
		else
		{
			// New frame count = maxNbFrame + "All"
			while ( _frameCount > p_newFrameCount )
			{
				removeAction( actions().last() );
				_frameCount--;
			}
		}
	}

	void TrajectoryFramesMenu::_updateFrameNames()
	{
		if ( actions().size() == 0 )
			return;

		const QList<QAction *> allActions  = actions();
		int					   actionIndex = 0;

		if ( allActions[ actionIndex ] == _allFramesAction )
			actionIndex++;

		while ( actionIndex < allActions.size() )
		{
			_updateFrameName( allActions[ actionIndex ] );
			actionIndex++;
		}
	}
	void TrajectoryFramesMenu::_updateFrameName( QAction * const p_action )
	{
		const int frameIndex = p_action->data().toInt();
		p_action->setText( _getFrameDisplayName( frameIndex ) );
	}
	QString TrajectoryFramesMenu::_getFrameDisplayName( const int p_frameIndex ) const
	{
		std::string frameName = _frameDisplayNameStr;
		Util::String::replaceAll( frameName, "#", std::to_string( p_frameIndex ) );

		return QString::fromStdString( frameName );
	}

	void TrajectoryFramesMenu::setDisplayAllOptionName( const std::string & p_name )
	{
		_allFramesAction->setText( QString::fromStdString( p_name ) );
	}

	void TrajectoryFramesMenu::setDisplayAllFramesOption( const bool p_displayAllFramesOption )
	{
		if ( _allFramesOption != p_displayAllFramesOption )
		{
			_allFramesOption = p_displayAllFramesOption;

			if ( _allFramesOption )
			{
				if ( actions().size() == 0 )
					addAction( _allFramesAction );
				else
					insertAction( *( actions().begin() ), _allFramesAction );
			}
		}
	}
	void TrajectoryFramesMenu::_onFrameSelected( const QAction * const p_action )
	{
		const int frameSelected = p_action->data().toInt();
		emit	  onFrameSelected( frameSelected );
	}

} // namespace VTX::UI::Widget::CustomWidget