#ifndef __VTX_UI_WIDGET_SCENE__
#define __VTX_UI_WIDGET_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include <QDockWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class SceneWidget : public BaseManualWidget<QDockWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~SceneWidget();
				void receiveEvent( const Event::VTXEvent & p_event ) override;
				void localize() override;

			  protected:
				SceneWidget( QWidget * p_parent );
				void setupUi( const QString & p_name ) override;
				void setupSlots() override;

			  private:
				QWidget *	  _verticalLayoutWidget;
				QVBoxLayout * _verticalLayout;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
