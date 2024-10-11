#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/settings.hpp"
#include <QCheckBox>
#include <QDockWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPointer>
#include <QToolButton>
#include <QVBoxLayout>

namespace VTX::UI::QT::DockWidget
{

	class Options : public Core::BaseDockWidget<Options>, public Savable
	{
	  public:
		Options( QWidget * p_parent );

		void save() override;
		void restore() override;

	  private:
		inline static const QString _SETTING_KEY_VSYNC = "options/vsync";

		// Graphics.
		QPointer<QCheckBox> _checkBoxVSync;

		// Cache.
		inline static const QString _TEXT_CACHE_COUNT = "Files : %1";
		inline static const QString _TEXT_CACHE_SIZE  = "Size : %1";

		QPointer<QLabel> _labelCacheCount;
		QPointer<QLabel> _labelCacheSize;

		void _refreshCacheInfos();
	};

} // namespace VTX::UI::QT::DockWidget

#endif
