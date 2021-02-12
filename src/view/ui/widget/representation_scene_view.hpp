#ifndef __VTX_VIEW_UI_WIDGET_REPRESENTATION_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_REPRESENTATION_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include "generic/base_representable.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "ui/widget/scene/scene_item_widget.hpp"
#include "view/base_view.hpp"
#include <QMenu>
#include <QTreeWidget>

namespace VTX::View::UI::Widget
{
	class RepresentationSceneView :
		public View::BaseView<Model::Representation::InstantiatedRepresentation>,
		public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_VIEW
		VTX_WIDGET

	  public:
		void localize() override;
		void notify( const Event::VTXEvent * const p_event ) override;

		void setTarget( Generic::BaseRepresentable & p_renderable );
		void updatePosInSceneHierarchy( const int p_position ) override;

		const Model::ID & getModelID() const override { return _model->getId(); };

	  protected:
		RepresentationSceneView( Model::Representation::InstantiatedRepresentation * const p_model,
								 QWidget * const										   p_parent ) :
			View::BaseView<Model::Representation::InstantiatedRepresentation>( p_model ),
			SceneItemWidget( p_parent ) {};

		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		void		_onItemClicked( QTreeWidgetItem *, int );
		void		_deleteAction() override;
		QMimeData * _getDataForDrag() override;

	  private:
		Generic::BaseRepresentable * _representable = nullptr;
	};

} // namespace VTX::View::UI::Widget

#endif
