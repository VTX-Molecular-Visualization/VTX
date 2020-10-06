#ifndef __VTX_UI_WIDGET_MENU_MAIN_MOLECULE__
#define __VTX_UI_WIDGET_MENU_MAIN_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "download_molecule_dialog.hpp"
#include "menu_toolblock_widget.hpp"
#include "menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainMoleculeWidget : public MenuToolBlockWidget
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~MenuMainMoleculeWidget();
				void localize() override;

			  protected:
				MenuMainMoleculeWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;

			  private:
				MenuToolButtonWidget * _loadMoleculeButton	   = nullptr;
				MenuToolButtonWidget * _downloadMoleculeButton = nullptr;
				MenuToolButtonWidget * _saveMoleculeButton	   = nullptr;

				DownloadMoleculeDialog * _downloadMoleculeDialog = nullptr;

			  private:
				void _loadMoleculeFile();
				void _downloadMoleculeFile();
				void _saveMoleculeFile();
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
