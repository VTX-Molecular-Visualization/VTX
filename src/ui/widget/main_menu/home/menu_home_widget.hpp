#ifndef __VTX_UI_WIDGET_MENU_HOME__
#define __VTX_UI_WIDGET_MENU_HOME__

#include "menu_home_molecule_widget.hpp"
#include "menu_home_session_widget.hpp"
#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
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
					class MenuHomeWidget : public MenuTooltabWidget
					{
						VTX_WIDGET

					  public:
						~MenuHomeWidget();
						void localize() override;

					  protected:
						MenuHomeWidget( QWidget * p_parent = nullptr );
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						MenuHomeSessionWidget *	 _session  = nullptr;
						MenuHomeMoleculeWidget * _molecule = nullptr;
					};
				} // namespace Home
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
