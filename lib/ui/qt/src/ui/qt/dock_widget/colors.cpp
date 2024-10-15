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

		// Atoms.
		auto * groupBoxAtoms = new QGroupBox( "Layout" );
		auto * layoutAtoms	 = new Layout::FlowLayout( groupBoxAtoms );
		// auto * layoutAtoms = new QVBoxLayout( groupBoxAtoms );

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
					auto dialog = QColorDialog( Helper::toQColor( component->getLayout().layout[ i ] ), this );
					// dialog->setOption( QColorDialog::ShowAlphaChannel );
					// dialog->setOption( QColorDialog::DontUseNativeDialog );
					dialog.exec();

					// Update color.
					_changeColor( i, dialog.currentColor() );
				}
			);

			layoutAtoms->addWidget( button );
		}

		_layout->addWidget( groupBoxAtoms );

		// Callbacks.
		component->onChange +=
			[ this, component ]( const size_t p_index ) { _refreshColor( component->getLayout(), p_index ); };
		component->onChangeAll += [ this, component ]() { _refreshColors( component->getLayout() ); };

		_refreshColors( component->getLayout() );
	}

	void Colors::_createGroupBox( std::string_view p_title, const size_t p_indexStart, const size_t p_indexEnd )
	{
		auto * groupBox = new QGroupBox( QString::fromStdString( p_title.data() ) );
		auto * layout	= new QVBoxLayout( groupBox );

		_layout->addWidget( groupBox );
	}

	void Colors::_refreshColors( const VTX::Core::Struct::ColorLayout & p_layout )
	{
		for ( size_t i = 0; i < p_layout.layout.size(); ++i )
		{
			_refreshColor( p_layout, i );
		}
	}

	void Colors::_refreshColor( const VTX::Core::Struct::ColorLayout & p_layout, const size_t p_index )
	{
		auto & color = p_layout.layout[ p_index ];

		QPalette palette = _buttons[ p_index ]->palette();
		palette.setColor( QPalette::Button, QColor( QString::fromStdString( color.toHexaString() ) ) );
		palette.setColor(
			QPalette::ButtonText,
			QColor( QString::fromStdString(
				color.brightness() > 0.5f ? COLOR_BLACK.toHexaString() : COLOR_WHITE.toHexaString()
			) )
		);
		_buttons[ p_index ]->setPalette( palette );
	}

	void Colors::_changeColor( const size_t p_index, const QColor & p_color )
	{
		App::ACTION_SYSTEM().execute<App::Action::Color::ChangeLayoutColor>(
			p_index, Util::Color::Rgba( p_color.redF(), p_color.greenF(), p_color.blueF() )
		);
	}
} // namespace VTX::UI::QT::DockWidget
