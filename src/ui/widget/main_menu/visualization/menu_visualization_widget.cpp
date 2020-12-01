#include "menu_visualization_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Visualization
				{
					MenuVisualizationWidget::MenuVisualizationWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
					MenuVisualizationWidget::~MenuVisualizationWidget() {}

					void MenuVisualizationWidget::_setupUi( const QString & p_name )
					{
						MenuTooltabWidget::_setupUi( p_name );

						_cameraAction	 = addToolBlock<MenuVisualizationCameraActionWidget>( "cameraActionBlockWidget" );
						_representation	 = addToolBlock<MenuVisualizationRepresentationWidget>( "RepresentationBlockWidget" );
						_objectDisplay	 = addToolBlock<MenuVisualizationObjectDisplayWidget>( "objectDisplayBlockWidget" );
						_renderEffects	 = addToolBlock<MenuVisualizationRenderEffectsWidget>( "renderEffectsBlockWidget" );
						_selectionAction = addToolBlock<MenuVisualizationSelectionActionWidget>( "selectionBlockWidget" );
						_windows		 = addToolBlock<MenuVisualizationWindowsWidget>( "windowsBlockWidget" );
					}
					void MenuVisualizationWidget::_setupSlots() {}
					void MenuVisualizationWidget::localize() {}
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
