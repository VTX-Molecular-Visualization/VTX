#ifndef __VTX_STYLE__
#define __VTX_STYLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <QIcon>

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

			const QIcon UNFOLDED_PIXMAP;
			const QIcon FOLDED_PIXMAP;
			const QIcon MOLECULE_SYMBOL;
			const QIcon CHAIN_SYMBOL;
			const QIcon RESIDUE_SYMBOL;
			const QIcon ATOM_SYMBOL;

		  private:
			inline IconConst() :
				UNFOLDED_PIXMAP( QIcon( ":/sprite/treeview_unfolded_molecule_icon.png" ) ), FOLDED_PIXMAP( QIcon( ":/sprite/treeview_folded_molecule_icon.png" ) ),
				MOLECULE_SYMBOL( QIcon( ":/sprite/symbol/molecule_symbol_icon.png" ) ), CHAIN_SYMBOL( QIcon( ":/sprite/symbol/chain_symbol_icon.png" ) ),
				RESIDUE_SYMBOL( QIcon( ":/sprite/symbol/residue_symbol_icon.png" ) ), ATOM_SYMBOL( QIcon( ":/sprite/symbol/atom_symbol_icon.png" ) ) {};
		};

		inline static const int INSPECTOR_HEADER_HEIGHT				= 30;
		inline static const int INSPECTOR_HEADER_ICON_SPACING		= 2;
		inline static const int INSPECTOR_HEADER_NAME_SPACING		= 10;
		inline static const int INSPECTOR_CONTENT_HORIZONTAL_OFFSET = 10;

	} // namespace Style
} // namespace VTX

#endif
