#include "menu_visualization_representation_widget.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationRepresentationWidget::MenuVisualizationRepresentationWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
	}
	MenuVisualizationRepresentationWidget::~MenuVisualizationRepresentationWidget() {}

	void MenuVisualizationRepresentationWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_buttons.clear();

		for ( int i = 0; i < Model::Representation::RepresentationLibrary::get().getRepresentationCount(); i++ )
		{
			const Model::Representation::BaseRepresentation * const representation
				= Model::Representation::RepresentationLibrary::get().getRepresentation( i );
			RepresentationPresetButton * const button
				= WidgetFactory::get().instantiateWidget<RepresentationPresetButton>( this,
																					  "applyVisualModelPresetButton" );
			button->setRepresentationID( i );
			button->setData( QString::fromStdString( representation->getName() ),
							 QString::fromStdString( representation->getIconPath() ),
							 Qt::Orientation::Horizontal );
			pushButton( *button, i / 3 );
			_buttons.emplace_back( button );
		}

		_addPreset
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "addNewVisualModelPresetButton" );
		_addPreset->setData( "Add\nPreset", ":/sprite/representation_new_preset_icon.png", Qt::Orientation::Vertical );
		pushButtonInNextColumn( *_addPreset );

		validate();
	}
	void MenuVisualizationRepresentationWidget::_setupSlots()
	{
		connect( _addPreset,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationRepresentationWidget::_addPresetAction );
	}

	void MenuVisualizationRepresentationWidget::localize() { setTitle( "Representation" ); }

	void MenuVisualizationRepresentationWidget::_addPresetAction()
	{
		VTXApp::get().getMainWindow().openSettingWindow( Settings::SETTING_MENU::REPRESENTATIONS );
	}

} // namespace VTX::UI::Widget::MainMenu::Visualization
