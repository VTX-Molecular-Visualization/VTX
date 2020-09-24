#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include "ui_inspector_widget.h"
#include <QDockWidget>


namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class InspectorWidget : public QDockWidget, public Event::BaseEventReceiverVTX
			{
				Q_OBJECT

			  public:
				  InspectorWidget( QWidget * p_parent );
				  ~InspectorWidget();

				virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				virtual std::vector<Event::VTX_EVENT> _getEvents() const override
				{
					return std::vector<Event::VTX_EVENT>( { Event::Global::ON_SCENE_CHANGE } );
				}

			  private:
				Ui_InspectorWidget * _ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
