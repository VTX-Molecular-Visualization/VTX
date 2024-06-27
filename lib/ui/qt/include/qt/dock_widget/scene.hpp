#ifndef __VTX_UI_QT_DOCK_WIDGET_SCENE__
#define __VTX_UI_QT_DOCK_WIDGET_SCENE__

#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Scene : public QDockWidget
	{
	  public:
		Scene( QWidget * p_parent ) : QDockWidget( "Scene", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~Scene() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
