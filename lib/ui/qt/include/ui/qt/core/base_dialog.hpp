#ifndef __VTX_UI_QT_CORE_BASE_DIALOG__
#define __VTX_UI_QT_CORE_BASE_DIALOG__

#include "ui/qt/widget/main_window.hpp"
#include <QDialog>
#include <concepts>

namespace VTX::UI::QT::Core
{

	template<typename D>
	concept ConceptDialog = std::is_base_of_v<QDialog, D>;

	/**
	 * @brief Abstract class that describes a dialog centered on the main window.
	 * @tparam T is the derived class type.
	 * @tparam D is the dialog type.
	 */
	template<typename T, ConceptDialog D = QDialog>
	class BaseDialog : public BaseWidget<T, D>
	{
	  public:
		template<typename... Args>
		BaseDialog( Args &&... p_args ) :
			BaseWidget<T, D>( WIDGETS::get().get<Widget::MainWindow>(), std::forward<Args>( p_args )... )
		{
		}

		virtual ~BaseDialog() = default;
	};
} // namespace VTX::UI::QT::Core

#endif
