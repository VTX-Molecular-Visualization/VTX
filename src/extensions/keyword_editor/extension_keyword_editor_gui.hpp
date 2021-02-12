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

	void populateInterfaceWithValues();

  private slots:
	void onOpenKeyfilePushButtonClicked();
	void onGenerateKeyfilePushButtonClicked();

	void comboBoxIntegratorIndexChanged( QString newSelectedItem );
	void comboBoxShortRangePolSolndexChanged( QString newSelectedItem );
	void comboBoxPolEquaIndexChanged( QString newSelectedItem );
	void comboBoxPeekStepIndexChanged( QString newSelectedItem );
	void comboBoxFittingIndexChanged( QString newSelectedItem );

  private:
	ExtensionKeywordEditor *	keywordEditor = nullptr;
	Ui::DockWidgetKeywordEditor ui;
};

#endif
