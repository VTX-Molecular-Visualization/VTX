#include "tool/old_tool/view/ui/widget/measurement/dihedral_angle_scene_view.hpp"
#include <app/action/label.hpp>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/mvc.hpp>
#include <ui/old_ui/ui/contextual_menu.hpp>
#include <ui/old_ui/ui/mime_type.hpp>
#include <ui/old_ui/ui/widget/contextual_menu/contextual_menu_label.hpp>
#include <ui/old_ui/ui/widget/contextual_menu/contextual_menu_selection.hpp>
#include <ui/old_ui/util/ui.hpp>
#include <ui/qt/action/label.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	DihedralAngleSceneView::DihedralAngleSceneView( Model::Measurement::DihedralAngle * const p_model,
													QWidget * const							  p_parent ) :
		App::Core::View::BaseView<Model::Measurement::DihedralAngle>( p_model ),
		SceneItemWidget( p_parent )
	{
	}

	void DihedralAngleSceneView::notify( const App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == App::Event::Model::DATA_CHANGE ) {}
		else if ( p_event->name == App::Event::Model::DISPLAY_NAME_CHANGE )
		{
			topLevelItem( 0 )->setText( 0, QString::fromStdString( _model->getDefaultName() ) );
		}
	}

	void DihedralAngleSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemChanged, this, &DihedralAngleSceneView::_onItemChanged );
		connect( this, &QTreeWidget::itemDoubleClicked, this, &DihedralAngleSceneView::_onItemDoubleClicked );
	}

	void DihedralAngleSceneView::_onItemChanged( QTreeWidgetItem * const p_item, const int p_column )
	{
		if ( p_column == 0 )
		{
			const std::string newName = p_item->text( 0 ).toStdString();

			if ( newName != _model->getDefaultName() )
			{
				VTX_ACTION( new App::Action::Label::Rename( *_model, newName ) );
			}
			else
			{
				const bool enableState = Util::UI::getCheckState( p_item->checkState( 0 ) );
				VTX_ACTION( new App::Action::Label::SetEnable( *_model, enableState ) );
			}
		}
	}

	void DihedralAngleSceneView::_onItemDoubleClicked( const QTreeWidgetItem * const p_item, const int p_column ) const
	{
		if ( p_column == 0 )
		{
			VTX_ACTION( new VTX::UI::QT::Action::Label::Orient( _model ) );
		}
	}

	void DihedralAngleSceneView::_fillItemSelection( const App::Application::Selection::SelectionModel & p_selection,
													 QItemSelection & p_itemSelection )
	{
		std::set<App::Component::Object3D::Label *> selectedLabels = std::set<App::Component::Object3D::Label *>();
		p_selection.getItemsOfType( VTX::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE, selectedLabels );

		bool pathItemAdded = false;

		for ( const App::Component::Object3D::Label * const label : selectedLabels )
		{
			if ( label == _model )
			{
				const QTreeWidgetItem * const labelItem = topLevelItem( 0 );

				if ( labelItem != nullptr )
					p_itemSelection.append( QItemSelectionRange( indexFromItem( labelItem ) ) );
			}
		}
	}

	void DihedralAngleSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos )
	{
		VTX::UI::ContextualMenu::Menu menuType	   = VTX::UI::ContextualMenu::Menu::COUNT;
		const QTreeWidgetItem * const targetedItem = itemAt( p_clicPos );

		if ( targetedItem == nullptr )
			return;

		const QPoint								  globalClicPos = mapToGlobal( p_clicPos );
		App::Application::Selection::SelectionModel & selection
			= App::Application::Selection::SelectionManager::get().getSelectionModel();

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
			VTX::UI::ContextualMenu::pop<App::Component::Object3D::Label>(
				VTX::UI::ContextualMenu::Menu::Label, _model, globalClicPos );
		}
	}
} // namespace VTX::View::UI::Widget::Measurement
