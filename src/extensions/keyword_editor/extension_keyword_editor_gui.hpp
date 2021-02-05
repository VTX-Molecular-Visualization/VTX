#ifndef KEYWORDEDITORFORM_H
#define KEYWORDEDITORFORM_H

#include "extension_keyword_editor.hpp"
#include "extensions/keyword_editor/forms/ui_keyword_editor_main_window.h"

class ExtensionKeywordEditorGUI : public QDockWidget
{
	Q_OBJECT

  public:
	explicit ExtensionKeywordEditorGUI( QWidget * parent = nullptr );
	void loadSettings();
	void saveSettings();

  private slots:
	void onOpenKeyfilePushButtonClicked();

  private:
	ExtensionKeywordEditor *	keywordEditor = nullptr;
	Ui::DockWidgetKeywordEditor ui;
};

#endif
