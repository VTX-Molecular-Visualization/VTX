#ifndef __VTX_UI_WIDGET_KEYWORDEDITOR__
#define __VTX_UI_WIDGET_KEYWORDEDITOR__

#include "extensions/keyword_editor/extension_keyword_editor.hpp"
#include "old_ui/ui/qt_form/ui_keyword_editor_main_window.h"
#include "old_ui/ui/widget/custom_widget/custom_qdockwidget.hpp"

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
					class ExtensionKeywordEditorGUI : public CustomQDockWidget
					{
						Q_OBJECT

					  public:
						explicit ExtensionKeywordEditorGUI( QWidget * parent );

						QString				moduleIcon() override;
						QString				moduleTitle() override;
						std::vector<bool> * moduleWindowProperties() override;
						QWidget *			moduleWidget() override;
						QString				moduleCitations() override;
						void				saveSettings( QSettings * savedSettings ) override;
						void				loadSettings( QSettings * settings ) override;

					  private:
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
						ExtensionKeywordEditor * keywordEditor = nullptr;
						Ui::KeywordEditorGUI	 ui;
					};
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX

#endif
