#include "distance_scene_view.hpp"
#include "action/action_manager.hpp"
#include "action/label.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "ui/contextual_menu.hpp"
#include "ui/mime_type.hpp"
#include "util/ui.hpp"
#include "ui/widget/contextual_menu/contextual_menu_label.hpp"
#include "ui/widget/contextual_menu/contextual_menu_selection.hpp"

namespace VTX::View::UI::Widget::Measurement
{
	DistanceSceneView::DistanceSceneView( Model::Measurement::Distance * const p_model, QWidget * const p_parent ) :
		View::BaseView<Model::Measurement::Distance>( p_model ), SceneItemWidget( p_parent )
	{
	}

	void DistanceSceneView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE ) {}
		else if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE )
		{
			topLevelItem( 0 )->setText( 0, QString::fromStdString( _model->getDefaultName() ) );
		}
	}

	void DistanceSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemChanged, this, &DistanceSceneView::_onItemChanged );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &DistanceSceneView::_onItemDoubleClicked );
	}

	void DistanceSceneView::_onItemChanged( QTreeWidgetItem * const p_item, const int p_column )
	{
		if ( p_column == 0 )
		{
			const std::string newName = p_item->text( 0 ).toStdString();

			if ( newName != _model->getDefaultName() )
			{
				VTX_ACTION( new Action::Label::Rename( *_model, newName ) );
			}
			else
			{
				const bool enableState = Util::UI::getCheckState(p_item->checkState( 0 ));
				VTX_ACTION( new Action::Label::SetEnable( *_model, enableState) );
			}
		}
	}

	void DistanceSceneView::_onItemDoubleClicked( const QTreeWidgetItem * const p_item, const int p_column ) const
	{
		if ( p_column == 0 )
		{
			VTX_ACTION( new Action::Label::Orient( _model ) );
		}
	}

	void DistanceSceneView::_fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection )
	{
		std::set<Model::Label *> selectedLabels = std::set<Model::Label *>();
		p_selection.getItemsOfType( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE, selectedLabels );

		bool pathItemAdded = false;

		for ( const Model::Label * const label : selectedLabels )
		{
			if ( label == _model )
			{
				const QTreeWidgetItem * const labelItem = topLevelItem( 0 );

				if ( labelItem != nullptr )
					p_itemSelection.append( QItemSelectionRange( indexFromItem( labelItem ) ) );
			}
		}
	}

	void DistanceSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos )
	{
		VTX::UI::ContextualMenu::Menu menuType	   = VTX::UI::ContextualMenu::Menu::COUNT;
		const QTreeWidgetItem * const targetedItem = itemAt( p_clicPos );

		if ( targetedItem == nullptr )
			return;

		const QPoint	   globalClicPos = mapToGlobal( p_clicPos );
		Model::Selection & selection	 = Selection::SelectionManager::get().getSelectionModel();

		if ( selection.isModelSelected( *_model ) )
		{
			VTX::UI::Widget::ContextualMenu::ContextualMenuSelection * const selectionContextualMenu
				= VTX::UI::ContextualMenu::getMenu<VTX::UI::Widget::ContextualMenu::ContextualMenuSelection>(
					VTX::UI::ContextualMenu::Menu::Selection );
			selectionContextualMenu->setFocusedTarget( _model );
			VTX::UI::ContextualMenu::pop( VTX::UI::ContextualMenu::Menu::Selection, &selection, globalClicPos );
		}
		else
		{
			VTX::UI::ContextualMenu::pop<Model::Label>( VTX::UI::ContextualMenu::Menu::Label, _model, globalClicPos );
		}
	}

	QMimeData * DistanceSceneView::_getDataForDrag() const { return VTX::UI::MimeType::generateLabelData( *_model ); }

} // namespace VTX::View::UI::Widget::Measurement
