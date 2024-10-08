#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include "ui/qt/base_widget.hpp"
#include <QCheckBox>
#include <QDockWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPointer>
#include <QToolButton>
#include <QVBoxLayout>

namespace VTX::UI::QT::DockWidget
{

	class Options : public BaseWidget<Options, QDockWidget>
	{
	  public:
		Options( QWidget * p_parent );

	  private:
		inline static const QString _TEXT_CACHE_COUNT = "Files : %1";
		inline static const QString _TEXT_CACHE_SIZE  = "Size : %1";

		QPointer<QLabel> _labelCacheCount;
		QPointer<QLabel> _labelCacheSize;

		void _refreshCacheInfos();
	};

} // namespace VTX::UI::QT::DockWidget

#endif
