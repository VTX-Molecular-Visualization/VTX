#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "id.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "view/ui/widget/base_widget_view.hpp"
#include <QDockWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Inspector
			{
				struct ViewData
				{
				  public:
					ViewData( const uint p_modelID, const ID::VTX_ID & p_view, QWidget * const p_widget ) : _modelID( p_modelID ), _viewID( p_view ), _widget( p_widget ) {};

					const uint		 _modelID;
					const ID::VTX_ID _viewID;
					QWidget * const	 _widget;
				};

				class InspectorWidget : public BaseManualWidget<QDockWidget>
				{
					VTX_WIDGET

				  public:
					~InspectorWidget();
					void localize() override;
					void receiveEvent( const Event::VTXEvent & p_event ) override;
					void refresh();
					void clear();

				  protected:
					InspectorWidget( QWidget * p_parent );

					void _addMolecule( Model::Molecule * const p_molecule );
					void _addRepresentation( Model::Representation::InstantiatedRepresentation * const p_representation );

					template<typename M,
							 typename V,
							 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
							 typename = std::enable_if<std::is_base_of<View::UI::Widget::BaseWidgetView<M, ViewItemWidget<M>>, V>::value>>
					void _addInspectorView( M * const p_model, const ID::VTX_ID & m_viewId )
					{
						V * const		inspectorView = MVC::MvcManager::get().instantiateViewWidget<V>( p_model, m_viewId, this );
						QWidget * const widget		  = inspectorView->getWidget();

						_inspectorViewsData.emplace_back( ViewData( p_model->getId(), m_viewId, widget ) );

						_verticalLayout->insertWidget( _verticalLayout->count() - 1, widget );
					}

					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QWidget *	  _scrollWidget	  = nullptr;
					QScrollArea * _scrollArea	  = nullptr;
					QVBoxLayout * _verticalLayout = nullptr;

					std::vector<ViewData> _inspectorViewsData = std::vector<ViewData>();
				};
			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
