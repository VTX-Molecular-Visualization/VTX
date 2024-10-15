#include "ui/qt/dock_widget/colors.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/layout/flow_layout.hpp"
#include <QColor>
#include <QColorDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <app/action/color.hpp>
#include <app/application/scene.hpp>
#include <app/component/representation/color_layout.hpp>
#include <core/struct/color_layout.hpp>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::DockWidget
{

	Colors::Colors( QWidget * p_parent ) : Core::BaseDockWidget<Colors>( "Colors", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		const auto & scene	   = App::SCENE();
		auto *		 component = &App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );

		// Search bar.
		auto * searchBar = new QLineEdit( _root );
		searchBar->setPlaceholderText( "Search..." );
		_layout->addWidget( searchBar );

		// Randomize button.
		auto * buttonRandomize = new QPushButton( "Randomize", this );
		_layout->addWidget( buttonRandomize );
		connect(
			buttonRandomize,
			&QPushButton::clicked,
			[ this ]() { App::ACTION_SYSTEM().execute<App::Action::Color::RandomizeLayoutColors>(); }
		);

		// Colors.
		auto * groupBoxColors = new QGroupBox( "Layout" );
		auto * layoutColors	  = new Layout::FlowLayout( groupBoxColors );
		// auto * layoutColors = new QVBoxLayout( groupBoxColors );

		// Create buttons.
		for ( size_t i = 0; i < VTX::Core::Struct::ColorLayout::LAYOUT_SIZE; ++i )
		{
			auto button = _buttons.emplace_back( new QPushButton( QString::number( i ), this ) );
			button->setFixedSize( 25, 25 );
			// button->setStyleSheet( "border: 0px" );
			// button->setFlat( true );
			// button->setAutoFillBackground( true );
			//  Set QPalette brush.
			// QPalette palette = button->palette();
			// palette.setBrush( QPalette::Button, QBrush( QColor( 255, 0, 0, 255 ) ) );

			// Connect picker.
			connect(
				button,
				&QPushButton::clicked,
				[ this, i, component ]()
				{
					assert( component );

					// Open button dialog.
					auto * dialog = new QColorDialog( Helper::toQColor( component->getLayout().layout[ i ] ), this );
					// dialog->setOption( QColorDialog::ShowAlphaChannel );
					// dialog->setOption( QColorDialog::DontUseNativeDialog );
					dialog->exec();

					// Update color.
					_changeColor( i, dialog->currentColor() );
				}
			);

			layoutColors->addWidget( button );
		}

		_layout->addWidget( groupBoxColors );

		// Callbacks.
		component->onChange +=
			[ this, component ]( const size_t p_index ) { _refreshColor( component->getLayout(), p_index ); };
		component->onChangeAll += [ this, component ]() { _refreshColors( component->getLayout() ); };

		_refreshColors( component->getLayout() );
	}

	void Colors::_refreshColors( const VTX::Core::Struct::ColorLayout & p_layout )
	{
		for ( size_t i = 0; i < p_layout.layout.size(); ++i )
		{
			_buttons[ i ]->setStyleSheet(
				QString( "background-color: " ) + QString::fromStdString( p_layout.layout[ i ].toHexaString() )
			);
		}
	}

	void Colors::_refreshColor( const VTX::Core::Struct::ColorLayout & p_layout, const size_t p_index )
	{
		_buttons[ p_index ]->setStyleSheet(
			QString( "background-color: " ) + QString::fromStdString( p_layout.layout[ p_index ].toHexaString() )
		);
	}

	void Colors::_changeColor( const size_t p_index, const QColor & p_color )
	{
		App::ACTION_SYSTEM().execute<App::Action::Color::ChangeLayoutColor>(
			p_index, Util::Color::Rgba( p_color.redF(), p_color.greenF(), p_color.blueF() )
		);
	}
} // namespace VTX::UI::QT::DockWidget
