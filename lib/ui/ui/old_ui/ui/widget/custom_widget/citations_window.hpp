#ifndef __VTX_UI_WIDGET_CITATIONS_WINDOW_
#define __VTX_UI_WIDGET_CITATIONS_WINDOW_

#include "old_ui/ui/qt_form/ui_citations_window.h"
#include <QDialog>
#include <QWidget>

class CitationsWindowWidget : public QDialog
{
	Q_OBJECT

  public:
	CitationsWindowWidget( QWidget * p_parent );
	~CitationsWindowWidget() {};

	Ui::DialogCitations ui;

  public slots:
	void onCopyCitationsToClipboardPressed();
};

#endif
