#include "ui/qt/dock_widget/colors.hpp"
#include <QColor>
#include <QColorDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <app/application/scene.hpp>
#include <app/component/representation/color_layout.hpp>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::DockWidget
{

	Colors::Colors( QWidget * p_parent ) : Core::BaseDockWidget<Colors>( "Colors", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// App::SCENE().onDefaultColorLayout += [ this ]( const App::Component::Representation::ColorLayout & p_layout )
		{
			//
		};

		auto & scene	   = App::SCENE();
		auto & colorLayout = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			App::ECS_REGISTRY().getEntity( scene )
		);

		auto & colors = colorLayout.getLayout().layout;

		// Colors.
		auto * groupBoxColors = new QGroupBox( "Colors" );
		auto * layoutColors	  = new QVBoxLayout( this );

		groupBoxColors->setLayout( layoutColors );

		for ( size_t i = 0; i < colors.size(); ++i )
		{
			auto * color = new QPushButton( QString::number( i ), this );
			// color->setStyleSheet( "background-color: " + colors[ i ].toQColor().name() );

			connect(
				color,
				&QPushButton::clicked,
				[ this, i ]()
				{
					/*
					auto & colors = colorLayout.getLayout().layout;

					QColor color = QColorDialog::getColor( colors[ i ].toQColor(), this );

					if ( color.isValid() )
					{
						colors[ i ] = Util::Color::Rgba( color.red(), color.green(), color.blue(), color.alpha() );
						color->setStyleSheet( "background-color: " + colors[ i ].toQColor().name() );
					}
					*/
				}
			);

			layoutColors->addWidget( color );
		}

		_layout->addWidget( groupBoxColors );
	}
} // namespace VTX::UI::QT::DockWidget
