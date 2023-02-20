#include "background_block.hpp"
#include "action/action_manager.hpp"
#include "action/renderer.hpp"
#include "id.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	BackgroundBlock::BackgroundBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
	BackgroundBlock::~BackgroundBlock() {}

	void BackgroundBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_backgroundBlackButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "backgroundBlackButton" );
		_backgroundBlackButton->setData( "Black", ":/sprite/background_black_icon.png", Qt::Orientation::Vertical );
		pushButton( *_backgroundBlackButton, 0 );

		_backgroundWhiteButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "backgroundWhiteButton" );
		_backgroundWhiteButton->setData( "White", ":/sprite/background_white_icon.png", Qt::Orientation::Vertical );
		pushButton( *_backgroundWhiteButton, 1 );

		validate();
	}

	void BackgroundBlock::localize() { setTitle( "Background" ); }

	void BackgroundBlock::_setupSlots()
	{
		_backgroundBlackButton->setTriggerAction( this, &BackgroundBlock::_setBackgroundToBlack );
		_backgroundWhiteButton->setTriggerAction( this, &BackgroundBlock::_setBackgroundToWhite );
	}

	void BackgroundBlock::_setBackgroundToBlack() const { _setBackgroundColor( Color::Rgba::BLACK ); }
	void BackgroundBlock::_setBackgroundToWhite() const { _setBackgroundColor( Color::Rgba::WHITE ); }

	void BackgroundBlock::_setBackgroundColor( const Color::Rgba & p_color ) const
	{
		VTX_ACTION( new Action::Renderer::ChangeBackgroundColor( VTX_RENDER_EFFECT(), p_color ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
