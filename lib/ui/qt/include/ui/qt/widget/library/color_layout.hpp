#ifndef __VTX_UI_QT_WIDGET_LIBRARY_COLOR_LAYOUT__
#define __VTX_UI_QT_WIDGET_LIBRARY_COLOR_LAYOUT__

#include "base_preset_widget.hpp"
#include "ui/qt/widget/color_picker.hpp"
#include <app/library/preset/color_layout.hpp>

namespace VTX::UI::QT::Widget::Library
{

	class ColorLayout final : public BasePresetWidget<App::Library::Preset::Representation>
	{
	  public:
	  protected:
	  private:
		/*
		std::vector<QPointer<Widget::ColorPicker>> _buttons;

		void _createGroupBox(
			const std::string_view				   p_title,
			const VTX::Core::Struct::ColorLayout & p_layout,
			const size_t						   p_start,
			const size_t						   p_count,
			const std::string_view * const		   p_text = nullptr,
			const std::string_view * const		   p_tip  = nullptr
		);

		void _refreshColors( const VTX::Core::Struct::ColorLayout & );
		void _refreshColor( const VTX::Core::Struct::ColorLayout &, const size_t );
		void _refreshButtonVisibility(
			const bool		   p_hide,
			const size_t	   p_start,
			const size_t	   p_count,
			const bool * const p_isCommonValues
		);

		void _changeColor( const size_t, const QColor & );
		*/
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
