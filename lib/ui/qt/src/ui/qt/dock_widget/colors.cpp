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
#include <core/chemdb/chain.hpp>
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
		_checkBoxHide = new QCheckBox( "Hide non usual", _root );
		_layout->addWidget( _checkBoxHide );
		connect(
			_checkBoxHide,
			&QCheckBox::stateChanged,
			[ this ]( const int p_state )
			{
				using namespace VTX::Core::ChemDB;
				const bool hide = p_state == Qt::Checked;
				_refreshButtonVisibility(
					hide, Color::LAYOUT_OFFSET_ATOMS, Color::LAYOUT_COUNT_ATOMS, Atom::SYMBOL_IS_COMMON
				);
				_refreshButtonVisibility(
					hide, Color::LAYOUT_OFFSET_RESIDUES, Color::LAYOUT_COUNT_RESIDUES, Residue::SYMBOL_IS_COMMON
				);
			}
		);

		// Randomize button.
		auto * buttonRandomize = new QPushButton( "Randomize", _root );
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
		_createGroupBox( "Chain", component->getLayout(), LAYOUT_OFFSET_CHAINS, LAYOUT_COUNT_CHAINS, Chain::NAME );
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
			// QString text = p_text ? QString::fromStdString( p_text[ offset ].data() ) : QString::number( i );

			_buttons[ i ] = new Widget::ColorPicker( Helper::toQColor( p_layout.layout[ i ] ), groupBox );
			_buttons[ i ]->setFixedSize( _BUTTON_SIZE, _BUTTON_SIZE );

			if ( p_text )
			{
				// First letter in uppercase.
				QString text = QString::fromStdString( p_text[ offset ].data() );
				text		 = text.toLower();
				text[ 0 ]	 = text[ 0 ].toUpper();
				_buttons[ i ]->setText( text );
			}

			if ( p_tip )
			{
				_buttons[ i ]->setToolTip( QString::fromStdString( p_tip[ offset ].data() ) );
				_buttons[ i ]->setStatusTip( QString::fromStdString( p_tip[ offset ].data() ) );
			}

			// Connect picker.
			_buttons[ i ]->onColorChanged += [ this, i ]( const QColor & p_color ) { _changeColor( i, p_color ); };

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
		_buttons[ p_index ]->setColor( Helper::toQColor( p_layout.layout[ p_index ] ) );
	}

	void Colors::_refreshButtonVisibility(
		const bool		   p_hide,
		const size_t	   p_start,
		const size_t	   p_count,
		const bool * const p_isCommonValues
	)
	{
		using namespace VTX::Core::ChemDB;

		size_t count = 0;
		for ( size_t i = p_start; i < p_start + p_count; ++i )
		{
			// Hide button.
			_buttons[ i ]->setVisible( not p_hide || p_isCommonValues[ count++ ] );
		}
	}

	void Colors::_changeColor( const size_t p_index, const QColor & p_color )
	{
		App::ACTION_SYSTEM().execute<App::Action::Color::ChangeLayoutColor>( p_index, Helper::fromQColor( p_color ) );
	}

	void Colors::save() { SETTINGS.setValue( _SETTING_KEY_HIDE, _checkBoxHide->isChecked() ); }

	void Colors::restore() { _checkBoxHide->setChecked( SETTINGS.value( _SETTING_KEY_HIDE, true ).toBool() ); }
} // namespace VTX::UI::QT::DockWidget
