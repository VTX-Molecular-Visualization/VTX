#include "ui/old_ui/ui/widget/main_menu/molecule/representation_block.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/old/application/representation/representation_library.hpp>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	RepresentationBlock::RepresentationBlock(
		App::Old::Application::Representation::RepresentationLibrary * const _representationLibrary,
		QWidget *														p_parent ) :
		App::Old::Core::View::BaseView<App::Old::Application::Representation::RepresentationLibrary>( _representationLibrary ),
		MenuToolBlockWidget( p_parent )
	{
	}
	RepresentationBlock::~RepresentationBlock() {}

	void RepresentationBlock::notify( const VTX::App::Old::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::App::Old::Event::Model::DATA_CHANGE )
		{
			_refreshView();
		}
		else if ( p_event->name == VTX::App::Old::Event::Model::QUICK_ACCESS_CHANGE )
		{
			_refreshView();
		}
		else if ( p_event->name == VTX::App::Old::Event::Model::DISPLAY_NAME_CHANGE )
		{
			_refreshNames();
		}
		else if ( p_event->name == VTX::App::Old::Event::Model::REPRESENTATION_TYPE_CHANGE )
		{
			_refreshIcons();
		}
	}

	void RepresentationBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );
		_instantiateUI();
	}
	void RepresentationBlock::_setupSlots()
	{
		connect( _addPreset, &MenuToolButtonWidget::clicked, this, &RepresentationBlock::_addPresetAction );
	}

	void RepresentationBlock::_refreshView()
	{
		reset();

		_instantiateUI();
		_setupSlots();
		localize();
	}

	void RepresentationBlock::_instantiateUI()
	{
		_buttons.clear();

		int quickAccessRepresentationCount = 0;
		for ( int i = 0; i < App::Old::Application::Representation::RepresentationLibrary::get().getRepresentationCount();
			  i++ )
		{
			const App::Old::Application::Representation::RepresentationPreset * const representation
				= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentation( i );

			if ( !representation->hasQuickAccess() )
				continue;

			RepresentationPresetButton * const button
				= WidgetFactory::get().instantiateWidget<RepresentationPresetButton>( this,
																					  "applyVisualModelPresetButton" );
			button->setRepresentationID( i );
			button->setData( QString::fromStdString( representation->getName() ),
							 *Style::IconConst::get().getRepresentationIcon( representation->getRepresentationType() ),
							 Qt::Orientation::Horizontal );
			pushButton( *button, quickAccessRepresentationCount / MAX_ROW_COUNT );
			_buttons.emplace( i, button );

			quickAccessRepresentationCount++;
		}

		for ( int i = 0; i < getColumnCount(); i++ )
			setRowCountInColumn( i, MAX_ROW_COUNT );

		_addPreset
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "addNewVisualModelPresetButton" );
		_addPreset->setData(
			"Preset\nSettings", ":/sprite/representation_settings_icon.png", Qt::Orientation::Vertical );
		pushButtonInNextColumn( *_addPreset );

		validate();
	}

	void RepresentationBlock::_refreshNames()
	{
		for ( int i = 0; i < App::Old::Application::Representation::RepresentationLibrary::get().getRepresentationCount();
			  i++ )
		{
			const App::Old::Application::Representation::RepresentationPreset * const representation
				= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentation( i );

			if ( !representation->hasQuickAccess() )
				continue;

			_buttons[ i ]->setName( QString::fromStdString( representation->getName() ) );
		}
	}
	void RepresentationBlock::_refreshIcons()
	{
		for ( int i = 0; i < App::Old::Application::Representation::RepresentationLibrary::get().getRepresentationCount();
			  i++ )
		{
			const App::Old::Application::Representation::RepresentationPreset * const representation
				= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentation( i );

			if ( !representation->hasQuickAccess() )
				continue;

			_buttons[ i ]->setIcon(
				*Style::IconConst::get().getRepresentationIcon( representation->getRepresentationType() ) );
		}
	}

	void RepresentationBlock::localize() { setTitle( "Representation" ); }

	void RepresentationBlock::_addPresetAction()
	{
		UI::VTXApp::get().getMainWindow().openSettingWindow( Settings::SETTING_MENU::REPRESENTATIONS );
	}

} // namespace VTX::UI::Widget::MainMenu::Molecule
