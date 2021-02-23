#ifndef __VTX_UI_WIDGET_CUSTOM_QDOCK_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_QDOCK_WIDGET__

#include "ui/widget/custom_widget/settings_window.hpp"
#include <QDockWidget>
#include <QWidget>

class CustomQDockWidget : public QDockWidget
{
	Q_OBJECT

  public:
	CustomQDockWidget( QWidget * p_parent );
	~CustomQDockWidget() {};

	void set_window_properties( bool savable, bool lockable );
	void save_settings();
	void load_settings();

  public slots:
	void onSettingsButtonClicked();

  private:
	SettingsWindowWidget * settingsWindow = nullptr;
};

#endif
