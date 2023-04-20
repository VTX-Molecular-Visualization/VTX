#include "tool/old_tool/view/ui/widget/measurement/distance_to_cycle_scene_view.hpp"
#include <app/core/action/action_manager.hpp>
#include <app/action/label.hpp>
#include <app/model/selection.hpp>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/selection/selection_manager.hpp>
#include <ui/old_ui/ui/contextual_menu.hpp>
#include <ui/old_ui/ui/mime_type.hpp>
#include <ui/old_ui/ui/widget/contextual_menu/contextual_menu_label.hpp>
#include <ui/old_ui/ui/widget/contextual_menu/contextual_menu_selection.hpp>
#include <ui/qt/action/label.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	DistanceToCycleSceneView::DistanceToCycleSceneView( Model::Measurement::DistanceToCycle * const p_model,
														QWidget * const								p_parent ) :
		View::BaseView<Model::Measurement::DistanceToCycle>( p_model ),
		SceneItemWidget( p_parent )
	{
	}

	void DistanceToCycleSceneView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE ) {}
		else if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE )
		{
			topLevelItem( 0 )->setText( 0, QString::fromStdString( _model->getDefaultName() ) );
		}
	}

	void DistanceToCycleSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemChanged, this, &DistanceToCycleSceneView::_onItemChanged );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &DistanceToCycleSceneView::_onItemDoubleClicked );
	}

	void DistanceToCycleSceneView::_onItemChanged( QTreeWidgetItem * const p_item, const int p_column )
	{
		if ( p_column == 0 )
		{
			const std::string newName = p_item->text( 0 ).toStdString();

			if ( newName != _model->getDefaultName() )
			{
				VTX_ACTION( new Action::Label::Rename( *_model, newName ) );
			}
		}
	}

	void DistanceToCycleSceneView::_onItemDoubleClicked( const QTreeWidgetItem * const p_item,
														 const int					   p_column ) const
	{
		if ( p_column == 0 )
		{
			VTX_ACTION( new VTX::UI::QT::Action::Label::Orient( _model ) );
		}
	}

	void DistanceToCycleSceneView::_fillItemSelection( const Model::Selection & p_selection,
													   QItemSelection &			p_itemSelection )
	{
		std::set<Model::Label *> selectedLabels = std::set<Model::Label *>();
		p_selection.getItemsOfType( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE, selectedLabels );

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

	void DistanceToCycleSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos )
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

} // namespace VTX::View::UI::Widget::Measurement
