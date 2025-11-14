#include "ui/qt/dock_widget/render_settings.hpp"
#include "ui/qt/widget/library/render_settings.hpp"
#include <util/factories.hpp>

namespace VTX::UI::QT::DockWidget
{

	RenderSettings::RenderSettings( QWidget * p_parent ) : BaseDockWidget( p_parent )
	{
		setWindowTitle( "Render settings" );
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		auto * const renderSettingsWidget = Util::Factories::newInit<Widget::Library::RenderSettings>( this );
		_layout->addWidget( renderSettingsWidget );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

} // namespace VTX::UI::QT::DockWidget
