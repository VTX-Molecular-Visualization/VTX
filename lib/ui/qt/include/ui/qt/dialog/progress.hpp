#ifndef __VTX_UI_QT_DIALOG_PROGRESS__
#define __VTX_UI_QT_DIALOG_PROGRESS__

#include "ui/qt/base_widget.hpp"
#include <QProgressDialog>
#include <functional>

namespace VTX::UI::QT::Dialog
{
	class Progress : public BaseWidget<Progress, QProgressDialog>
	{
	  public:
		Progress( const std::string_view p_text, std::optional<std::function<void( void )>> p_onCancel = std::nullopt );

		inline void setValue( const float p_value )
		{
			setMaximum( 100 );
			QProgressDialog::setValue( p_value * 100 );
		}
	};
} // namespace VTX::UI::QT::Dialog

#endif
