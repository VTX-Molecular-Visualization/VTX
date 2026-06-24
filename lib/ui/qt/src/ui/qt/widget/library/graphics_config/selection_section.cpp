#include "ui/qt/widget/library/graphics_config/selection_section.hpp"
#include "ui/qt/helper.hpp"
#include <QSignalBlocker>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	SelectionSection::SelectionSection( QWidget * const p_parent ) : Section( "Selection", p_parent )
	{
		using namespace Renderer;
		_colorPicker = new ColorPicker( this );
		addWidget( _colorPicker );
		connect(
			this,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_SELECTION, bool>( p_state ); }
		);
		_colorPicker->onColorChanged += [ this ]( const QColor & p_color )
		{
			_changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_SELECTION, Util::Color::Rgba>( Helper::fromQColor( p_color ) );
		};
	}

	void SelectionSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		const QSignalBlocker blockerGroup( this );
		const QSignalBlocker blockerColor( _colorPicker );
		const auto			 config = p_config.selection.value_or( VTX::Renderer::GraphicsConfigs::SELECTION_DEFAULT );
		setChecked( p_config.selection.has_value() );
		_colorPicker->setColor( Helper::toQColor( config.color ) );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
