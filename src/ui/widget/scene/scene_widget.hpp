#ifndef __VTX_UI_WIDGET_SCENE__
#define __VTX_UI_WIDGET_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QDockWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QVariant>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Scene
			{
				class SceneWidget : public BaseManualWidget<QDockWidget>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void receiveEvent( const Event::VTXEvent & p_event ) override;
					void localize() override;

				  protected:
					SceneWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QWidget *	  _scrollAreaContent = nullptr;
					QVBoxLayout * _layout			 = nullptr;
				};

			} // namespace Scene
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
