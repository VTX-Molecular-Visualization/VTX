#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include "qt/base_widget.hpp"
#include <QCheckBox>
#include <QDockWidget>
#include <QGroupBox>
#include <QToolButton>
#include <QVBoxLayout>

namespace VTX::UI::QT::DockWidget
{

	class Options : public BaseWidget<Options, QDockWidget>, public Savable
	{
	  public:
		Options( QWidget * p_parent ) : BaseWidget<Options, QDockWidget>( "Options", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~Options() {}

		void save() override {}

		void restore() override {}

	  private:
	};

} // namespace VTX::UI::QT::DockWidget

#endif
