#ifndef __VTX_UI_CONST__
#define __VTX_UI_CONST__

#ifdef _MSC_VER
#pragma once
#endif

#include <QIcon>

namespace VTX
{
	namespace UI
	{
		class UiConsts
		{
		  public:
			inline static UiConsts & get()
			{
				static UiConsts instance;
				return instance;
			};

			const QIcon UNFOLDED_PIXMAP;
			const QIcon FOLDED_PIXMAP;

		  private:
			inline UiConsts() :
				UNFOLDED_PIXMAP( QIcon( ":/sprite/treeview_unfolded_molecule_icon.png" ) ), FOLDED_PIXMAP( QIcon( ":/sprite/treeview_folded_molecule_icon.png" ) ) {};
		};
	} // namespace UI
} // namespace VTX

#endif
