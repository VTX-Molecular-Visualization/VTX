#ifndef __VTX_UI_WIDGET_SCENE__
#define __VTX_UI_WIDGET_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_scene_widget.h"
#include <QDockWidget>


namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class SceneWidget : public BaseWidget<QDockWidget, Ui_SceneWidget>
			{
				Q_OBJECT

			  public:
				SceneWidget( QWidget * p_parent );
				
				virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			  private:
				Ui_SceneWidget * _ui;
			};
		} // namespace Widget
	}// namespace UI
} // namespace VTX
#endif
