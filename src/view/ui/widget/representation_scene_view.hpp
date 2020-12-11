#ifndef __VTX_VIEW_UI_WIDGET_REPRESENTATION_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_REPRESENTATION_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include "generic/base_representable.hpp"
#include "model/chain.hpp"
#include "model/representation/representation.hpp"
#include "model/residue.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "view/base_view.hpp"
#include <QMenu>
#include <QTreeWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class RepresentationSceneView : public View::BaseView<Model::Representation::BaseRepresentation>, public VTX::UI::Widget::BaseManualWidget<QTreeWidget>
				{
					VTX_VIEW
					VTX_WIDGET

				  public:
					void localize() override;
					void notify( const Event::VTXEvent * const p_event ) override;

					void setTarget( Generic::BaseRepresentable & p_renderable );

				  protected:
					RepresentationSceneView( Model::Representation::BaseRepresentation * const p_model, QWidget * const p_parent ) :
						View::BaseView<Model::Representation::BaseRepresentation>( p_model ), BaseManualWidget( p_parent ) {};

					void _setupUi( const QString & ) override;
					void _setupSlots() override;

					void _onItemClicked( QTreeWidgetItem *, int );

				  private:
					QMenu * _contextMenu = nullptr;

					const Qt::CheckState _getCheckState( const bool p_enable ) const { return p_enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked; };
					void				 _onCustomContextMenuCalled( const QPoint & p_clicPos );
					void				 _deleteAction();

					Generic::BaseRepresentable * _representable = nullptr;
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
