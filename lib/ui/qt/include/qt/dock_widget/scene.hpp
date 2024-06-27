#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include "qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Scene : public BaseWidget<Scene, QDockWidget>
	{
	  public:
		Scene( QWidget * p_parent ) : BaseWidget<Scene, QDockWidget>( "Scene", p_parent )
		{
			setObjectName( "Scene" );
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~Scene() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
