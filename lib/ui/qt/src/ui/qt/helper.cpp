#include "ui/qt/helper.hpp"
#include "ui/qt/events.hpp"
#include <QAction>
#include <QPointer>
#include <QPushButton>
#include <QWidget>
#include <app/helper/script.hpp>
#include <app/services.hpp>
#include <exception>
#include <util/event_hub.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Helper
{
	void centerWidget( QWidget & p_widget, const QRect & p_rect )
	{
		const int x = ( p_rect.width() - p_widget.width() ) / 2;
		const int y = ( p_rect.height() - p_widget.height() ) / 2;
		p_widget.move( x, y );
	}

	QColor toQColor( const Util::Color::Rgba & p_color )
	{
		// return QColor( p_color.x * 255, p_color.y * 255, p_color.z * 255, p_color.w * 255 );
		return QColor::fromRgbF( p_color.r(), p_color.g(), p_color.b(), p_color.a() );
	}

	void duplicateScript( const FilePath & p_path )
	{
		try
		{
			App::HUB().trigger<Events::ScriptCopied>( App::Helper::Script::duplicate( p_path ) );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Unable to copy script {}: {}", p_path.string(), p_e.what() );
		}
	}

	QPushButton * makeActionButton( QAction & p_action, QWidget * const p_parent )
	{
		QPushButton * const button = makeQObject<QPushButton>( p_parent );
		const QPointer<QAction> action( &p_action );

		const auto sync = [ button, action ]()
		{
			if ( action.isNull() )
			{
				return;
			}

			button->setText( action->text() );
			button->setIcon( action->icon() );
			button->setToolTip( action->toolTip() );
			button->setEnabled( action->isEnabled() );
			button->setCheckable( action->isCheckable() );
			button->setChecked( action->isChecked() );
		};
		sync();

		QObject::connect( &p_action, &QAction::changed, button, sync );
		QObject::connect( button, &QPushButton::clicked, &p_action, &QAction::trigger );
		return button;
	}

	Util::Color::Rgba fromQColor( const QColor & p_color )
	{ return Util::Color::Rgba( p_color.redF(), p_color.greenF(), p_color.blueF(), p_color.alphaF() ); }

} // namespace VTX::UI::QT::Helper
