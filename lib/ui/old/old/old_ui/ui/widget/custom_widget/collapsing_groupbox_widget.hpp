#ifndef __VTX_UI_WIDGET_COLLAPSING_GROUPBOX_WIDGET__
#define __VTX_UI_WIDGET_COLLAPSING_GROUPBOX_WIDGET__

#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class CollapsingGroupboxWidget : public QGroupBox
{
	Q_OBJECT

  public:
	CollapsingGroupboxWidget( QWidget * p_parent );
	~CollapsingGroupboxWidget() {};

	void collapse();
	void expand();

  public slots:
	void toggleBodyVisibility( bool checked );
};
#endif
