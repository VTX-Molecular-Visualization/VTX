#include "representation_scene_view.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "action/selection.hpp"
#include "model/selection.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"
#include "ui/mime_type.hpp"

namespace VTX::View::UI::Widget
{
	void RepresentationSceneView::notify( const Event::VTXEvent * const p_event ) {}

	void RepresentationSceneView::_setupUi( const QString & p_name )
	{
		SceneItemWidget::_setupUi( p_name );

		QTreeWidgetItem * const representationView = new QTreeWidgetItem();

		representationView->setText( 0, QString::fromStdString( _model->getName() ) );
		representationView->setIcon( 0, VTX::Style::IconConst::get().REPRESENTATION_SYMBOL );
		representationView->setCheckState( 0, Qt::CheckState::Checked );

		addTopLevelItem( representationView );
	}
	void RepresentationSceneView::_setupSlots()
	{
		SceneItemWidget::_setupSlots();

		connect( this, &QTreeWidget::itemClicked, this, &RepresentationSceneView::_onItemClicked );
	}
	void RepresentationSceneView::localize() {}

	void RepresentationSceneView::_onItemClicked( QTreeWidgetItem * p_item, int p_column )
	{
		Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::SelectRepresentation( selectionModel, *_model ) );
	}

	void RepresentationSceneView::setTarget( Generic::BaseRepresentable & p_renderable )
	{
		if ( _representable != nullptr )
			Representation::RepresentationManager::get().removeRepresentation( _model, _representable, false );

		_representable = &p_renderable;

		if ( _representable != nullptr )
			Representation::RepresentationManager::get().addToRepresentation( _model, _representable );
	}
	void RepresentationSceneView::updatePosInSceneHierarchy( const int p_position )
	{
		SceneItemWidget::updatePosInSceneHierarchy( p_position );
		_model->setPriority( p_position );
	}

	void		RepresentationSceneView::_deleteAction() { VTX_ACTION( new Action::DeleteRepresentation( _model ) ); }
	QMimeData * RepresentationSceneView::_getDataForDrag()
	{
		return VTX::UI::MimeType::generateInstantiatedRepresentationData( *_model );
	};

} // namespace VTX::View::UI::Widget
