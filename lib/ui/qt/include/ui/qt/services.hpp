#ifndef __VTX_UI_QT_SERVICES__
#define __VTX_UI_QT_SERVICES__

#include <QPointer>
#include <app/ecs.hpp>

namespace VTX::UI::QT
{
	namespace Widget
	{
		class MainWindow;
	}

	inline QPointer<Widget::MainWindow> MAIN_WINDOW() { return App::ECS::getCtx<QPointer<Widget::MainWindow>>(); }
} // namespace VTX::UI::QT

#endif
