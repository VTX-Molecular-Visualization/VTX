#ifndef __VTX_STYLE__
#define __VTX_STYLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "id.hpp"
//#include "vtx_app.hpp"
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

	} // namespace Style
} // namespace VTX

#endif
