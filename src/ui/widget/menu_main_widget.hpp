#ifndef __VTX_UI_WIDGET_MENU_MAIN__
#define __VTX_UI_WIDGET_MENU_MAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_main_molecule_widget.hpp"
#include "menu_main_session_widget.hpp"
#include "menu_tooltab_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainWidget : public MenuTooltabWidget
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~MenuMainWidget();
				void localize() override;

			  protected:
				MenuMainWidget( QWidget * p_parent = nullptr );
				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;

			  private:
				MenuMainSessionWidget *	 _session  = nullptr;
				MenuMainMoleculeWidget * _molecule = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
