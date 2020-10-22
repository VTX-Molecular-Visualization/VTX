#ifndef __VTX_UI_WIDGET_MENU_TOOLTAB__
#define __VTX_UI_WIDGET_MENU_TOOLTAB__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_toolblock_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <string>
#include <type_traits>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				class MenuTooltabWidget : public BaseManualWidget<QWidget>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					~MenuTooltabWidget();

					template<typename W, typename = std::enable_if<std::is_base_of<W, Widget::BaseManualWidgetInitializer>::value>>
					W * addToolBlock( const std::string & p_name )
					{
						if ( _horizontalLayout->count() > 1 )
							_addSeparator();

						W * toolblock = WidgetFactory::get().GetWidget<W>( this, p_name );
						_horizontalLayout->insertWidget( _horizontalLayout->count() - 1, toolblock );

						return toolblock;
					}

				  protected:
					MenuTooltabWidget( QWidget * p_parent = nullptr );
					void _setupUi( const QString & p_name ) override;
					void _addSeparator();

				  private:
					QHBoxLayout * _horizontalLayout = nullptr;
				};
			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
