#include "menu_visualization_representation_widget.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationRepresentationWidget::MenuVisualizationRepresentationWidget(
		Model::Representation::RepresentationLibrary * const _representationLibrary,
		QWidget *											 p_parent ) :
		View::BaseView<Model::Representation::RepresentationLibrary>( _representationLibrary ),
		MenuToolBlockWidget( p_parent )
	{
	}
	MenuVisualizationRepresentationWidget::~MenuVisualizationRepresentationWidget() {}

	void MenuVisualizationRepresentationWidget::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
		{
			_refreshView();
		}
		else if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE )
		{
			_refreshNames();
		}
		else if ( p_event->name == Event::Model::QUICK_ACCESS_CHANGE )
		{
			_refreshView();
		}
	}

	void MenuVisualizationRepresentationWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );
		_refreshView();
	}
	void MenuVisualizationRepresentationWidget::_setupSlots()
	{
		connect( _addPreset,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationRepresentationWidget::_addPresetAction );
	}

	void MenuVisualizationRepresentationWidget::_refreshView()
	{
		reset();

		_buttons.clear();

		int quickAccessRepresentationCount = 0;
		for ( int i = 0; i < Model::Representation::RepresentationLibrary::get().getRepresentationCount(); i++ )
		{
			const Model::Representation::Representation * const representation
				= Model::Representation::RepresentationLibrary::get().getRepresentation( i );

			if ( !representation->hasQuickAccess() )
				continue;

			RepresentationPresetButton * const button
				= WidgetFactory::get().instantiateWidget<RepresentationPresetButton>( this,
																					  "applyVisualModelPresetButton" );
			button->setRepresentationID( i );
			button->setData( QString::fromStdString( representation->getName() ),
							 QString::fromStdString( representation->getIconPath() ),
							 Qt::Orientation::Horizontal );
			pushButton( *button, quickAccessRepresentationCount / 3 );
			_buttons.emplace( i, button );

			quickAccessRepresentationCount++;
		}

		_addPreset
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "addNewVisualModelPresetButton" );
		_addPreset->setData(
			"Preset\nSettings", ":/sprite/representation_new_preset_icon.png", Qt::Orientation::Vertical );
		pushButtonInNextColumn( *_addPreset );

		_setupSlots();

		validate();
	}
	void MenuVisualizationRepresentationWidget::_refreshNames()
	{
		for ( int i = 0; i < Model::Representation::RepresentationLibrary::get().getRepresentationCount(); i++ )
		{
			const Model::Representation::Representation * const representation
				= Model::Representation::RepresentationLibrary::get().getRepresentation( i );

			if ( !representation->hasQuickAccess() )
				continue;

			_buttons[ i ]->setName( QString::fromStdString( representation->getName() ) );
		}
	}

	void MenuVisualizationRepresentationWidget::localize() { setTitle( "Representation" ); }

	void MenuVisualizationRepresentationWidget::_addPresetAction()
	{
		VTXApp::get().getMainWindow().openSettingWindow( Settings::SETTING_MENU::REPRESENTATIONS );
	}

} // namespace VTX::UI::Widget::MainMenu::Visualization
