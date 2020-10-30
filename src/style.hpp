#ifndef __VTX_STYLE__
#define __VTX_STYLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "id.hpp"
//#include "vtx_app.hpp"
#include "selection/selection_enum.hpp"
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QPixmap>
#include <iostream>

namespace VTX
{
	namespace Style
	{
		enum class SYMBOL_DISPLAY_MODE : int
		{
			SHORT,
			LONG
		};

		class IconConst
		{
		  public:
			inline static IconConst & get()
			{
				static IconConst instance;
				return instance;
			};

			const QPixmap UNFOLDED_PIXMAP;
			const QPixmap FOLDED_PIXMAP;
			const QPixmap MOLECULE_SYMBOL;
			const QPixmap CHAIN_SYMBOL;
			const QPixmap RESIDUE_SYMBOL;
			const QPixmap ATOM_SYMBOL;

			const QPixmap * const getModelSymbol( const ID::VTX_ID & p_id ) const
			{
				const QPixmap * res;

				if ( p_id == ID::Model::MODEL_MOLECULE )
					res = &MOLECULE_SYMBOL;
				else if ( p_id == ID::Model::MODEL_CHAIN )
					res = &CHAIN_SYMBOL;
				else if ( p_id == ID::Model::MODEL_RESIDUE )
					res = &RESIDUE_SYMBOL;
				else if ( p_id == ID::Model::MODEL_ATOM )
					res = &ATOM_SYMBOL;
				else
				{
					std::cout << "[WARNING] - Symbol for model " + p_id + " not managed in IconConst::getModelSymbol." << std::endl;
					// VTX_WARNING( "Symbol for model " + p_id + " not managed in IconConst::getModelSymbol." );
					res = nullptr;
				}

				return res;
			};

		  private:
			inline IconConst() :
				UNFOLDED_PIXMAP( QPixmap( ":/sprite/treeview_unfolded_molecule_icon.png" ) ), FOLDED_PIXMAP( QPixmap( ":/sprite/treeview_folded_molecule_icon.png" ) ),
				MOLECULE_SYMBOL( QPixmap( ":/sprite/symbol/molecule_symbol_icon.png" ) ), CHAIN_SYMBOL( QPixmap( ":/sprite/symbol/chain_symbol_icon.png" ) ),
				RESIDUE_SYMBOL( QPixmap( ":/sprite/symbol/residue_symbol_icon.png" ) ), ATOM_SYMBOL( QPixmap( ":/sprite/symbol/atom_symbol_icon.png" ) ) {};
		};

		inline static const int INSPECTOR_HEADER_HEIGHT				= 30;
		inline static const int INSPECTOR_HEADER_NAME_SPACING		= 10;
		inline static const int INSPECTOR_CONTENT_HORIZONTAL_OFFSET = 10;

		inline static const std::string SEQUENCE_CHAIN_NAME_SEPARATOR = "/";
		inline static const int			SEQUENCE_CHAIN_SCALE_STEP	  = 5;

		inline static const VTX::Selection::SelectionType SELECTION_WINDOW_DEFAULT_SELECTION = VTX::Selection::SelectionType::RESIDUE;

		inline static const QFont TOOL_MENU_BUTTON_FONT = QFont( "Helvetica", 10 );
		inline static const QFont SEQUENCE_DISPLAY_FONT = QFont( "Courier", 10 );

		inline static const QColor BLACK_COLOR			  = QColor( 0, 0, 0 );
		inline static const QColor BACKGROUND_COLOR		  = QColor( 34, 34, 34 );
		inline static const QColor HIGHLIGHTED_COLOR	  = QColor( 85, 85, 85 );
		inline static const QColor TEXT_COLOR			  = QColor( 255, 255, 255 );
		inline static const QColor PLACEHOLDER_TEXT_COLOR = QColor( 200, 200, 200 );

		inline static const void applyApplicationPaletteInPalette( QPalette & p_palette )
		{
			p_palette.setColor( QPalette::ColorRole::Window, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::WindowText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::WindowText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::Base, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::Text, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::PlaceholderText, PLACEHOLDER_TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::ToolTipBase, BLACK_COLOR );
			p_palette.setColor( QPalette::ColorRole::ToolTipText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::Button, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::ButtonText, TEXT_COLOR );
			p_palette.setColor( QPalette::ColorRole::BrightText, TEXT_COLOR );

			p_palette.setColor( QPalette::ColorRole::Light, HIGHLIGHTED_COLOR );
			p_palette.setColor( QPalette::ColorRole::Midlight, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::Mid, BACKGROUND_COLOR );
			p_palette.setColor( QPalette::ColorRole::Shadow, BLACK_COLOR );
			p_palette.setColor( QPalette::ColorRole::Dark, BLACK_COLOR );

			p_palette.setColor( QPalette::ColorRole::Highlight, HIGHLIGHTED_COLOR );
		};

	} // namespace Style
} // namespace VTX

#endif
