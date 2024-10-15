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
#include <core/chemdb/color.hpp>
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

		// Checkbox to hide non usual items.
		_checkBoxHide = new QCheckBox( "Hide non usual", this );
		_layout->addWidget( _checkBoxHide );
		connect( _checkBoxHide, &QCheckBox::stateChanged, this, &Colors::_refreshButtonVisibility );

		// Randomize button.
		auto * buttonRandomize = new QPushButton( "Randomize", this );
		_layout->addWidget( buttonRandomize );
		connect(
			buttonRandomize,
			&QPushButton::clicked,
			[ this ]() { App::ACTION_SYSTEM().execute<App::Action::Color::RandomizeLayoutColors>(); }
		);

		using namespace VTX::Core::Struct;
		using namespace VTX::Core::ChemDB;
		using namespace VTX::Core::ChemDB::Color;

		// Buttons.
		_buttons.resize( VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );

		// Group boxes.
		_createGroupBox(
			"Atom", component->getLayout(), LAYOUT_OFFSET_ATOMS, LAYOUT_COUNT_ATOMS, Atom::SYMBOL_STR, Atom::SYMBOL_NAME
		);
		_createGroupBox(
			"Residue",
			component->getLayout(),
			LAYOUT_OFFSET_RESIDUES,
			LAYOUT_COUNT_RESIDUES,
			Residue::SYMBOL_STR,
			Residue::SYMBOL_NAME
		);
		_createGroupBox( "Chain", component->getLayout(), LAYOUT_OFFSET_CHAINS, LAYOUT_COUNT_CHAINS );
		_createGroupBox( "Ribbon", component->getLayout(), LAYOUT_OFFSET_RIBBONS, LAYOUT_COUNT_RIBBONS );
		_createGroupBox( "Custom", component->getLayout(), LAYOUT_OFFSET_CUSTOM, LAYOUT_COUNT_CUSTOM );

		// Spacer.
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		// Callbacks.
		component->onChange +=
			[ this, component ]( const size_t p_index ) { _refreshColor( component->getLayout(), p_index ); };
		component->onChangeAll += [ this, component ]() { _refreshColors( component->getLayout() ); };

		_refreshColors( component->getLayout() );
		_checkBoxHide->setChecked( true );
	}

	void Colors::_createGroupBox(
		const std::string_view				   p_title,
		const VTX::Core::Struct::ColorLayout & p_layout,
		const size_t						   p_start,
		const size_t						   p_count,
		const std::string_view * const		   p_text,
		const std::string_view * const		   p_tip
	)
	{
		auto * groupBox = new QGroupBox( QString::fromStdString( p_title.data() ) );
		auto * layout	= new Layout::FlowLayout( groupBox );

		// Create buttons.
		size_t offset = 0;
		for ( size_t i = p_start; i <= p_start + p_count - 1; ++i )
		{
			QString text = p_text ? QString::fromStdString( p_text[ offset ].data() ) : QString::number( i );

			_buttons[ i ] = new QPushButton( text, this );
			_buttons[ i ]->setFixedSize( _BUTTON_SIZE, _BUTTON_SIZE );

			if ( p_tip )
			{
				_buttons[ i ]->setToolTip( QString::fromStdString( p_tip[ offset ].data() ) );
				_buttons[ i ]->setStatusTip( QString::fromStdString( p_tip[ offset ].data() ) );
			}

			// Connect picker.
			connect(
				_buttons[ i ],
				&QPushButton::clicked,
				[ this, i, p_layout ]()
				{
					// Open button dialog.
					QColorDialog dialog( Helper::toQColor( p_layout.layout[ i ] ), this );
					// dialog->setOption( QColorDialog::ShowAlphaChannel );
					// dialog->setOption( QColorDialog::DontUseNativeDialog );
					dialog.exec();

					if ( dialog.result() == QDialog::Accepted )
					{
						// Update color.
						_changeColor( i, dialog.currentColor() );
					}
				}
			);

			layout->addWidget( _buttons[ i ] );
			offset++;
		}

		_layout->addWidget( groupBox );
	}

	void Colors::_refreshColors( const VTX::Core::Struct::ColorLayout & p_layout )
	{
		for ( size_t i = 0; i < p_layout.layout.size(); ++i )
		{
			if ( _buttons[ i ] )
			{
				_refreshColor( p_layout, i );
			}
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

	void Colors::_refreshButtonVisibility( const int p_state )
	{
		using namespace VTX::Core::ChemDB::Color;
		using namespace VTX::Core::ChemDB::Atom;

		const bool hideNonUsual = p_state == Qt::Checked;

		// Atoms.
		size_t a = 0;
		for ( size_t i = LAYOUT_OFFSET_ATOMS; i < LAYOUT_COUNT_ATOMS; ++i )
		{
			_buttons[ i ]->setVisible( not hideNonUsual || SYMBOL_IS_COMMON[ a++ ] );
		}
	}

	void Colors::_changeColor( const size_t p_index, const QColor & p_color )
	{
		App::ACTION_SYSTEM().execute<App::Action::Color::ChangeLayoutColor>(
			p_index, Util::Color::Rgba( p_color.redF(), p_color.greenF(), p_color.blueF() )
		);
	}

	void Colors::save() { SETTINGS.setValue( _SETTING_KEY_HIDE, _checkBoxHide->isChecked() ); }

	void Colors::restore() { _checkBoxHide->setChecked( SETTINGS.value( _SETTING_KEY_HIDE, true ).toBool() ); }
} // namespace VTX::UI::QT::DockWidget
