#include "ui/old_ui/ui/widget/main_menu/molecule/toolbar.hpp"
#include "ui/id.hpp"
#include "ui/old_ui/ui/widget/main_menu/molecule/object_display_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/molecule/representation_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/molecule/selection_action_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/molecule/windows_block.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <app/application/render_effect/render_effect_library.hpp>
#include <app/application/representation/representation_library.hpp>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	Toolbar::Toolbar( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	Toolbar::~Toolbar() {}

	void Toolbar::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		addToolBlock<ObjectDisplayBlock>( "objectDisplayBlock" );
		addToolBlock<SelectionActionBlock>( "selectionBlock" );
		RepresentationBlock * const representationBlock
			= WidgetFactory::get().instantiateViewWidget<RepresentationBlock>(
				&App::Application::Representation::RepresentationLibrary::get(),
				UI::ID::View::UI_MENU_VISUALIZATION_REPRESENTATION_LIBRARY,
				this,
				"representationBlock" );
		addToolBlock( representationBlock );

		addToolBlock<WindowsBlock>( "windowsBlock" );
	}
	void Toolbar::_setupSlots() {}
	void Toolbar::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Molecule
