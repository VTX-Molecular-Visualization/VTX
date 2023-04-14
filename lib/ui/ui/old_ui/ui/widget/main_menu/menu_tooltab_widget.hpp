#ifndef __VTX_UI_WIDGET_MENU_TOOLTAB__
#define __VTX_UI_WIDGET_MENU_TOOLTAB__

#include "menu_toolblock_widget.hpp"
#include "old_ui/ui/widget/base_manual_widget.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <string>
#include <type_traits>
#include <vector>

namespace VTX::UI::Widget::MainMenu
{
	class MenuTooltabWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

	  public:
		~MenuTooltabWidget();

		template<typename W, typename = std::enable_if<std::is_base_of<MenuToolBlockWidget, W>::value>>
		W * addToolBlock( const std::string & p_name )
		{
			W * toolblock = WidgetFactory::get().instantiateWidget<W>( this, p_name );
			addToolBlock( toolblock );

			return toolblock;
		}

		void addToolBlock( MenuToolBlockWidget * const p_widget );

		const MenuToolBlockWidget * const getPreviousToolBlock( const MenuToolBlockWidget * const p_from,
																const bool						  p_loop = true ) const;
		const MenuToolBlockWidget * const getNextToolBlock( const MenuToolBlockWidget * const p_from,
															const bool						  p_loop = true ) const;

	  protected:
		MenuTooltabWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _addSeparator();

	  private:
		QHBoxLayout * _horizontalLayout = nullptr;

		std::vector<MenuToolBlockWidget *> _toolBlocks = std::vector<MenuToolBlockWidget *>();
	};
} // namespace VTX::UI::Widget::MainMenu
#endif
