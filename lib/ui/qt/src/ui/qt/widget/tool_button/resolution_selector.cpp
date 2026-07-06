#include "ui/qt/tool_button/resolution_selector.hpp"
#include <QLineEdit>
#include <QMenu>
#include <QRegularExpression>
#include <QWidgetAction>
#include <array>
#include <string>
#include <util/resolution.hpp>

namespace
{
	const std::array<VTX::Util::Resolution, 7> SCREEN_RESOLUTION_PRESETS = {
		VTX::Util::ResolutionPreset::HD,	  VTX::Util::ResolutionPreset::WXGA,  VTX::Util::ResolutionPreset::HD_PLUS,
		VTX::Util::ResolutionPreset::FULL_HD, VTX::Util::ResolutionPreset::WUXGA, VTX::Util::ResolutionPreset::WQHD,
		VTX::Util::ResolutionPreset::UHD_4K,
	};

	constexpr size_t CUSTOM_RESOLUTION_MAX_WIDTH  = 3840;
	constexpr size_t CUSTOM_RESOLUTION_MAX_HEIGHT = 2160;
} // namespace

// namespace

namespace VTX::UI::QT::ToolButton
{
	ResolutionSelector::ResolutionSelector( QWidget * p_parent ) : QToolButton( p_parent )
	{
		setPopupMode( QToolButton::InstantPopup );
		setAutoRaise( true );
		setToolButtonStyle( Qt::ToolButtonTextOnly );

		auto * const menu = new QMenu( this );

		for ( const Util::Resolution & preset : SCREEN_RESOLUTION_PRESETS )
		{
			const size_t width	= preset.width;
			const size_t height = preset.height;

			QAction * const action = menu->addAction(
				QString( "%1 (%2x%3)" ).arg( QString::fromStdString( preset.name ) ).arg( width ).arg( height )
			);

			connect(
				action,
				&QAction::triggered,
				this,
				[ this, width, height ]() { emit resolutionChanged( width, height ); }
			);
		}

		menu->addSeparator();

		auto * const customResolutionAction = new QWidgetAction( menu );
		auto * const customResolutionInput	= new QLineEdit( menu );
		customResolutionInput->setPlaceholderText( "1920 1080" );
		customResolutionInput->setClearButtonEnabled( true );
		customResolutionAction->setDefaultWidget( customResolutionInput );
		menu->addAction( customResolutionAction );

		connect(
			customResolutionInput,
			&QLineEdit::returnPressed,
			this,
			[ this, menu, customResolutionInput ]()
			{
				static const QRegularExpression numberRegex( "\\d+" );

				size_t values[ 2 ] {};
				int	   count = 0;

				QRegularExpressionMatchIterator it = numberRegex.globalMatch( customResolutionInput->text() );
				while ( it.hasNext() && count < 2 )
				{
					bool					 ok	   = false;
					const unsigned long long value = it.next().captured().toULongLong( &ok );
					if ( not ok || value == 0 )
					{
						return;
					}

					values[ count++ ] = value;
				}

				if ( count == 1 )
				{
					values[ 1 ] = values[ 0 ];
				}
				else if ( count != 2 )
				{
					return;
				}

				if ( values[ 0 ] > CUSTOM_RESOLUTION_MAX_WIDTH || values[ 1 ] > CUSTOM_RESOLUTION_MAX_HEIGHT )
				{
					return;
				}

				emit resolutionChanged( values[ 0 ], values[ 1 ] );
				menu->hide();
			}
		);

		setMenu( menu );
	}

	void ResolutionSelector::setResolution( const size_t p_width, const size_t p_height )
	{ setText( QString( "%1x%2" ).arg( p_width ).arg( p_height ) ); }

} // namespace VTX::UI::QT::ToolButton
