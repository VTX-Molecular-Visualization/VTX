#include "toolbar.hpp"
#include "object_display_block.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include "representation_block.hpp"
#include "selection_action_block.hpp"
#include "windows_block.hpp"
#include <app/old_app/model/renderer/render_effect_preset_library.hpp>
#include <app/old_app/model/representation/representation_library.hpp>

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
				&Model::Representation::RepresentationLibrary::get(),
				ID::View::UI_MENU_VISUALIZATION_REPRESENTATION_LIBRARY,
				this,
				"representationBlock" );
		addToolBlock( representationBlock );

		addToolBlock<WindowsBlock>( "windowsBlock" );
	}
	void Toolbar::_setupSlots() {}
	void Toolbar::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Molecule
