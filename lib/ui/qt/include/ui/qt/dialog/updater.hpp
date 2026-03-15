#ifndef __VTX_UI_QT_DIALOG_UPDATER__
#define __VTX_UI_QT_DIALOG_UPDATER__

#include "ui/qt/dialog/base_dialog.hpp"

namespace VTX::UI::QT::Dialog
{

	class Updater : public BaseDialog<Updater>
	{
	  public:
		Updater( const App::Events::UpdateAvailable & );
	};

} // namespace VTX::UI::QT::Dialog

#endif
