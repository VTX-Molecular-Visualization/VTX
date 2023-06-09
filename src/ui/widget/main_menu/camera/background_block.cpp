#include "background_block.hpp"
#include "action/action_manager.hpp"
#include "action/renderer.hpp"
#include "id.hpp"
#include "ui/widget/renderer/default_background.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	BackgroundBlock::BackgroundBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
	BackgroundBlock::~BackgroundBlock() {}

	void BackgroundBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		const int backgroundCount = int( Renderer::DEFAULT_BACKGROUND::COUNT );

		_buttons.resize( backgroundCount );
		for ( int i = 0; i < backgroundCount; i++ )
		{
			MenuToolButtonWidget * const button
				= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "changeBackgroundColorButton" );

			button->setData( QString::fromStdString( Renderer::DEFAULT_BACKGROUND_STR[ i ] ),
							 QString::fromStdString( Renderer::DEFAULT_BACKGROUND_ICON_PATH[ i ] ),
							 Qt::Orientation::Vertical );
			button->setExtraData( QVariant( i ) );

			pushButton( *button, i );
			_buttons[ i ] = button;
		}

		validate();
	}

	void BackgroundBlock::localize() { setTitle( "Background" ); }

	void BackgroundBlock::_setupSlots()
	{
		for ( MenuToolButtonWidget * button : _buttons )
		{
			button->setTriggerAction( this, &BackgroundBlock::_changeBackgroundColorActionTriggered );
		}
	}

	void BackgroundBlock::_changeBackgroundColorActionTriggered( const QVariant & p_extraData )
	{
		const int			defaultBeckgroundIndex = p_extraData.toInt();
		const Color::Rgba & p_color				   = Renderer::DEFAULT_BACKGROUND_COLORS[ defaultBeckgroundIndex ];
		_setBackgroundColor( p_color );
	}

	void BackgroundBlock::_setBackgroundColor( const Color::Rgba & p_color ) const
	{
		VTX_ACTION( new Action::Renderer::ChangeBackgroundColor( VTX_RENDER_EFFECT(), p_color ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
