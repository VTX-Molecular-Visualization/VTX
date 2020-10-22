#ifndef __VTX_UI_WIDGET_MENU_HOME_MOLECULE__
#define __VTX_UI_WIDGET_MENU_HOME_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/dialog/download_molecule_dialog.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Home
				{
					class MenuHomeMoleculeWidget : public MenuToolBlockWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuHomeMoleculeWidget();
						void localize() override;

					  protected:
						MenuHomeMoleculeWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						MenuToolButtonWidget * _loadMoleculeButton	   = nullptr;
						MenuToolButtonWidget * _downloadMoleculeButton = nullptr;
						MenuToolButtonWidget * _saveMoleculeButton	   = nullptr;

						Dialog::DownloadMoleculeDialog * _downloadMoleculeDialog = nullptr;

					  private:
						void _loadMoleculeFile();
						void _downloadMoleculeFile();
						void _saveMoleculeFile();
					};
				} // namespace Home
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
