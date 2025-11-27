#ifndef __VTX_UI_QT_WIDGET_LIBRARY_COLOR_LAYOUT__
#define __VTX_UI_QT_WIDGET_LIBRARY_COLOR_LAYOUT__

#include "base_preset_widget.hpp"
#include "ui/qt/widget/color_picker.hpp"

namespace VTX::UI::QT::Widget::Library
{

	class ColorLayout final : public BasePresetWidget<Renderer::Color::Layout>
	{
	  public:
		ColorLayout( QWidget * p_parent );

		/**
		 * @brief Show/hide non usual color buttons.
		 */
		void refreshVisibility( const bool p_hide );

	  protected:
		// void _onPresetAdded( const std::string_view ) override;
		// void _onPresetChanged() override;

	  private:
		inline static const int _BUTTON_SIZE = 32;

		std::vector<QPointer<ColorPicker>> _buttons;

		void _createGroupBox(
			const std::string_view		   p_title,
			const size_t				   p_start,
			const size_t				   p_count,
			const std::string_view * const p_text = nullptr,
			const std::string_view * const p_tip  = nullptr
		);

		/**
		 * @brief Update app from widget.
		 */
		void _changeColor( const size_t, const QColor & );

		/**
		 * @brief Update widget from app.
		 */
		void _updateColor( const size_t, const Util::Color::Rgba & );

		void _update( App::ECS::Entity ) override;

		void _refreshButtonVisibility(
			const bool		   p_hide,
			const size_t	   p_start,
			const size_t	   p_count,
			const bool * const p_isCommonValues
		);

		// void _onCurrentPresetUpdated()
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
