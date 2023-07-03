#ifndef __VTX_UI_WIDGET_STATUS_BAR__
#define __VTX_UI_WIDGET_STATUS_BAR__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace StatusBar
			{
				class StatusBarWidget : public BaseManualWidget<QStatusBar>
				{
					VTX_WIDGET

				  public:
					void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;
					void localize() override;

				  protected:
					StatusBarWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QProgressBar * _progressBar = nullptr;
					QLabel *	   _infoLabel	= nullptr;
				};
			} // namespace StatusBar
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
