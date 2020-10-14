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

			const QIcon UNFOLDED_PIXMAP = QIcon( ":/sprite/treeview_unfolded_molecule_icon.png" );
			const QIcon FOLDED_PIXMAP	= QIcon( ":/sprite/treeview_folded_molecule_icon.png" );

		  private:
			inline IconConst() :
				UNFOLDED_PIXMAP( QIcon( ":/sprite/treeview_unfolded_molecule_icon.png" ) ), FOLDED_PIXMAP( QIcon( ":/sprite/treeview_folded_molecule_icon.png" ) ) {};
		};

		inline static const int INSPECTOR_HEADER_HEIGHT				= 30;
		inline static const int INSPECTOR_HEADER_ICON_SPACING		= 2;
		inline static const int INSPECTOR_HEADER_NAME_SPACING		= 10;
		inline static const int INSPECTOR_CONTENT_HORIZONTAL_OFFSET = 10;

	} // namespace Style
} // namespace VTX

#endif
