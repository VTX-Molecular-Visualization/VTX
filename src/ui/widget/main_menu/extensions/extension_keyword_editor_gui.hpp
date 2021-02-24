#ifndef __VTX_UI_WIDGET_KEYWORDEDITOR__
#define __VTX_UI_WIDGET_KEYWORDEDITOR__

//#ifdef _MSC_VER
//#pragma once
//#endif

#include "extensions/keyword_editor/extension_keyword_editor.hpp"
#include "ui/qt_form/ui_keyword_editor_main_window.h"
#include "ui/widget/custom_widget/settings_window.hpp"
#include <QDockWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Extensions
				{
					class ExtensionKeywordEditorGUI : public QDockWidget
					{
						Q_OBJECT

					  public:
						explicit ExtensionKeywordEditorGUI( QWidget * parent );

						void populateInterfaceWithValues();

					  private slots:
						void onExitButtonClicked();
						void onSettingsButtonClicked();
						void saveSettingsClicked();
						void loadSettingsClicked();
						void removeSettingsClicked();

						void onOpenKeyfilePushButtonClicked();
						void onGenerateKeyfilePushButtonClicked();

						void comboBoxIntegratorIndexChanged( QString newSelectedItem );
						void comboBoxShortRangePolSolndexChanged( QString newSelectedItem );
						void comboBoxPolEquaIndexChanged( QString newSelectedItem );
						void comboBoxPeekStepIndexChanged( QString newSelectedItem );
						void comboBoxFittingIndexChanged( QString newSelectedItem );

					  private:
						ExtensionKeywordEditor * keywordEditor = nullptr;

						SettingsWindowWidget * settingsWindow = nullptr;
						Ui::DockWidget		   ui;
					};
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX

#endif
