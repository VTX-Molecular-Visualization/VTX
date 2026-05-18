#include "ui/qt/tool_button/resolution_selector.hpp"
#include <QMenu>

namespace
{
	struct ResolutionPreset
	{
		std::string_view name;
		size_t			 width;
		size_t			 height;
	};

	constexpr std::array<ResolutionPreset, 7> RESOLUTION_PRESETS {
		ResolutionPreset { "HD", 1280, 720 },	   ResolutionPreset { "WXGA", 1280, 800 },
		ResolutionPreset { "HD+", 1600, 900 },	   ResolutionPreset { "Full HD", 1920, 1080 },
		ResolutionPreset { "WUXGA", 1920, 1200 },  ResolutionPreset { "WQHD", 2560, 1440 },
		ResolutionPreset { "4K UHD", 3840, 2160 },
	};
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

		for ( const ResolutionPreset & preset : RESOLUTION_PRESETS )
		{
			QAction * const action = menu->addAction( QString( "%1 (%2x%3)" )
														  .arg( QString::fromStdString( std::string( preset.name ) ) )
														  .arg( preset.width )
														  .arg( preset.height ) );

			connect(
				action,
				&QAction::triggered,
				this,
				[ this, &preset ]() { emit resolutionChanged( preset.width, preset.height ); }
			);
		}

		setMenu( menu );
	}

	void ResolutionSelector::setResolution( const size_t p_width, const size_t p_height )
	{
		setText( QString( "%1x%2" ).arg( p_width ).arg( p_height ) );
	}

} // namespace VTX::UI::QT::ToolButton
