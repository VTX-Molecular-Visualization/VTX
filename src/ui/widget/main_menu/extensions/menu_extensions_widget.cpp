#include "menu_extensions_widget.hpp"

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
					MenuExtensionsWidget::MenuExtensionsWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
					MenuExtensionsWidget::~MenuExtensionsWidget() {}

					void MenuExtensionsWidget::_setupUi( const QString & p_name )
					{
						MenuTooltabWidget::_setupUi( p_name );

						_allExtensions = addToolBlock<MenuExtensionsAllWidgets>( "allWidgetsBlockWidget" );
						_jobs		   = addToolBlock<MenuExtensionsJobsWidgets>( "jobsWidgetsBlockWidget" );
					}
					void MenuExtensionsWidget::_setupSlots() {}
					void MenuExtensionsWidget::localize() {}
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
