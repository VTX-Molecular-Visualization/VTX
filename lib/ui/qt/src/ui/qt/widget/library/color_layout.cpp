#include "ui/qt/widget/library/color_layout.hpp"
#include "ui/qt/core/layout/flow_layout.hpp"
#include "ui/qt/helper.hpp"
#include <app/action/color_layout.hpp>
#include <core/chemdb/atom.hpp>
#include <core/chemdb/chain.hpp>
#include <core/chemdb/color_layout.hpp>
#include <core/chemdb/residue.hpp>

namespace VTX::UI::QT::Widget::Library
{

	ColorLayout::ColorLayout( QWidget * p_parent ) : BasePresetWidget( p_parent )
	{
		using namespace VTX::Core::Struct;
		using namespace App::Library::Preset;
		using namespace Core::Widget;
		using namespace VTX::Core::ChemDB;
		using namespace VTX::Core::ChemDB::ColorLayout;

		setTitle( "Edit color layout" );

		// Search bar.
		auto * searchBar = new QLineEdit( this );
		searchBar->setPlaceholderText( "TODO" );
		addWidget( searchBar );

		_buttons.resize( VTX::Core::Struct::COLOR_LAYOUT_SIZE );

		// Group boxes.
		_createGroupBox( "Atom", LAYOUT_OFFSET_ATOMS, LAYOUT_COUNT_ATOMS, Atom::SYMBOL_STR, Atom::SYMBOL_NAME );
		_createGroupBox(
			"Residue",

			LAYOUT_OFFSET_RESIDUES,
			LAYOUT_COUNT_RESIDUES,
			Residue::SYMBOL_STR,
			Residue::SYMBOL_NAME
		);
		_createGroupBox( "Chain", LAYOUT_OFFSET_CHAINS, LAYOUT_COUNT_CHAINS, Chain::NAME );
		_createGroupBox( "Ribbon", LAYOUT_OFFSET_RIBBONS, LAYOUT_COUNT_RIBBONS );
		_createGroupBox( "Custom", LAYOUT_OFFSET_CUSTOM, LAYOUT_COUNT_CUSTOM );
	}

	void ColorLayout::_onPresetAdded( const std::string_view p_name )
	{
		auto * const preset = _library->getPreset( p_name );

		preset->onChange += [ this, preset ]( const Index p_index )
		{
			if ( _preset != preset )
				return;

			_setColor( p_index, _preset->getData().colors[ p_index ] );
		};

		preset->onChangeAll += [ this, preset ]()
		{
			if ( _preset != preset )
				return;

			for ( size_t i = 0; i < VTX::Core::Struct::COLOR_LAYOUT_SIZE; ++i )
			{
				_setColor( i, _preset->getData().colors[ i ] );
			}
		};
	}

	void ColorLayout::_onPresetChanged()
	{
		assert( _preset != nullptr );

		for ( size_t i = 0; i < VTX::Core::Struct::COLOR_LAYOUT_SIZE; ++i )
		{
			_setColor( i, _preset->getData().colors[ i ] );
		}
	}

	void ColorLayout::refreshVisibility( const bool p_hide )
	{
		using namespace VTX::Core::ChemDB;
		using namespace VTX::Core::ChemDB::ColorLayout;

		_refreshButtonVisibility( p_hide, LAYOUT_OFFSET_ATOMS, LAYOUT_COUNT_ATOMS, Atom::SYMBOL_IS_COMMON );
		_refreshButtonVisibility( p_hide, LAYOUT_OFFSET_RESIDUES, LAYOUT_COUNT_RESIDUES, Residue::SYMBOL_IS_COMMON );
	}

	void ColorLayout::_createGroupBox(
		const std::string_view		   p_title,
		const size_t				   p_start,
		const size_t				   p_count,
		const std::string_view * const p_text,
		const std::string_view * const p_tip
	)
	{
		auto * groupBox = new QGroupBox( QString::fromStdString( p_title.data() ) );
		auto * layout	= new Core::Layout::FlowLayout( groupBox );

		// Create buttons.
		size_t offset = 0;
		for ( size_t i = p_start; i <= p_start + p_count - 1; ++i )
		{
			// QString text = p_text ? QString::fromStdString( p_text[ offset ].data() ) : QString::number( i );

			_buttons[ i ] = new Core::Widget::ColorPicker( groupBox );
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

		addWidget( groupBox );
	}

	void ColorLayout::_changeColor( const size_t p_index, const QColor & p_color )
	{
		App::ACTION_SYSTEM().execute<App::Action::ColorLayout::Change>(
			_preset, Index( p_index ), Helper::fromQColor( p_color )
		);
	}

	void ColorLayout::_setColor( const size_t p_index, const Util::Color::Rgba & p_color )
	{
		if ( _buttons[ p_index ] )
		{
			const QSignalBlocker blocker( _buttons[ p_index ] );
			_buttons[ p_index ]->setColor( Helper::toQColor( p_color ) );
		}
	}

	void ColorLayout::_refreshButtonVisibility(
		const bool		   p_hide,
		const size_t	   p_start,
		const size_t	   p_count,
		const bool * const p_isCommonValues
	)
	{
		using namespace VTX::Core::ChemDB;

		auto * groupBox = static_cast<QGroupBox *>( _buttons[ p_start ]->parentWidget() );
		delete groupBox->layout();
		auto * layout = new Core::Layout::FlowLayout( groupBox );

		size_t count = 0;
		for ( size_t i = p_start; i < p_start + p_count; ++i )
		{
			const bool show = not p_hide or p_isCommonValues[ count++ ];

			// Hide button.
			_buttons[ i ]->setVisible( show );
			if ( show )
			{
				layout->addWidget( _buttons[ i ] );
			}
		}
	}

} // namespace VTX::UI::QT::Widget::Library
