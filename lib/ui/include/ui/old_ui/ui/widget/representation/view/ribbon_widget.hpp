#ifndef __VTX_UI_WIDGET_REPRESENTATION_VIEW_RIBBON_WIDGET__
#define __VTX_UI_WIDGET_REPRESENTATION_VIEW_RIBBON_WIDGET__

#include "base_representation_view_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/old_ui/ui/widget/custom_widget/ribbon_color_mode_field_widget.hpp"

namespace VTX::UI::Widget::Representation::View
{
	class RibbonWidget : public BaseRepresentationViewWidget
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		void refresh( const App::Application::Representation::InstantiatedRepresentation &		 p_representation,
					  const std::unordered_set<const App::Application::Representation::BaseRepresentable *> & p_targets ) override;
		void updateWithNewValue( const App::Application::Representation::InstantiatedRepresentation &		p_representation,
								 const std::unordered_set<const App::Application::Representation::BaseRepresentable *> & p_targets ) override;
		void resetState() override;

	  signals:
		void onColorChange( const Color::Rgba & p_color );
		void onColorModeChange( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_newMode );
		void onColorBlendingModeChange( const Generic::COLOR_BLENDING_MODE & p_newMode );

	  protected:
		RibbonWidget( QWidget * p_parent = nullptr ) : BaseRepresentationViewWidget( p_parent ) {}
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QLabel * _colorModeLabel		 = nullptr;
		QLabel * _colorBlendingModeLabel = nullptr;

		CustomWidget::RibbonColorModeFieldWidget * _colorModeWidget			= nullptr;
		CustomWidget::QComboBoxMultiField *		   _colorBlendingModeWidget = nullptr;

		void _onColorChange( const Color::Rgba & p_color );
		void _onColorModeChange( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_newMode );
		void _onColorBlendingModeChange( const int p_newMode );
	};
} // namespace VTX::UI::Widget::Representation::View
#endif
