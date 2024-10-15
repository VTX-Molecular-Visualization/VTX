#include "ui/qt/dock_widget/colors.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/layout/flow_layout.hpp"
#include <QColor>
#include <QColorDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <app/application/scene.hpp>
#include <app/component/representation/color_layout.hpp>
#include <core/struct/color_layout.hpp>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::DockWidget
{

	Colors::Colors( QWidget * p_parent ) : Core::BaseDockWidget<Colors>( "Colors", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		const auto & scene = App::SCENE();
		_component		   = &App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
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
			[ this ]()
			{
				std::vector<Util::Color::Rgba> randomColors( VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );
				std::generate( randomColors.begin(), randomColors.end(), [] { return Util::Color::Rgba::random(); } );
				_component->setColors( randomColors );
			}
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
			// Connect picker.
			connect(
				button,
				&QPushButton::clicked,
				[ this, i ]()
				{
					assert( _component );

					// Open button dialog.
					auto * dialog = new QColorDialog( Helper::toQColor( _component->getLayout().layout[ i ] ), this );
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
		_component->onChange += [ this ]( const size_t p_index ) { _refreshColor( p_index ); };
		_component->onChangeAll += [ this ]() { _refreshColors(); };

		_refreshColors();
	}

	void Colors::_refreshColors()
	{
		for ( size_t i = 0; i < _component->getLayout().layout.size(); ++i )
		{
			_buttons[ i ]->setStyleSheet(
				QString( "background-color: " )
				+ QString::fromStdString( _component->getLayout().layout[ i ].toHexaString() )
			);
		}
	}

	void Colors::_refreshColor( const size_t p_index )
	{
		_buttons[ p_index ]->setStyleSheet(
			QString( "background-color: " )
			+ QString::fromStdString( _component->getLayout().layout[ p_index ].toHexaString() )
		);
	}

	// TODO: use action.
	void Colors::_changeColor( const size_t p_index, const QColor & p_color )
	{
		_component->setColor( p_index, Util::Color::Rgba( p_color.redF(), p_color.greenF(), p_color.blueF() ) );
	}
} // namespace VTX::UI::QT::DockWidget
