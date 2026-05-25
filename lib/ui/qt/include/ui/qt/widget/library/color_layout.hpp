#ifndef __VTX_UI_QT_WIDGET_LIBRARY_COLOR_LAYOUT__
#define __VTX_UI_QT_WIDGET_LIBRARY_COLOR_LAYOUT__

#include "base_preset_widget.hpp"
#include "ui/qt/widget/color_picker.hpp"
#include <QCheckBox>
#include <span>

namespace VTX::UI::QT::Widget::Library
{

	class ColorLayout final : public BasePresetWidget<VTX::Renderer::Color::Layout>
	{
	  public:
		ColorLayout( QWidget * p_parent );

		/**
		 * @brief Show/hide non usual color buttons.
		 */
		void refreshVisibility( const bool p_hide );

	  protected:
		/**
		 * @brief Update the widget when the preset is updated from App.
		 */
		void _update( App::Entity ) override;

	  private:
		inline static const int _BUTTON_SIZE = 32;

		QPointer<QCheckBox> _checkBoxHideNonUsual;
		std::vector<QPointer<ColorPicker>> _buttons;

		void _createGroupBox(
			const std::string_view			  p_title,
			const size_t					  p_start,
			const size_t					  p_count,
			std::span<const std::string_view> p_text = {},
			std::span<const std::string_view> p_tip	 = {}
		);

		/**
		 * @brief Update app from widget.
		 */
		void _changeColor( const size_t, const QColor & );

		/**
		 * @brief Update widget from app.
		 */
		void _updateColor( const size_t, const Util::Color::Rgba & );

		void _refreshButtonVisibility(
			const bool			  p_hide,
			const size_t		  p_start,
			const size_t		  p_count,
			std::span<const bool> p_isCommonValues
		);

		// void _onCurrentPresetUpdated()
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
