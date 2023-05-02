#ifndef __VTX_UI_WIDGET_REPRESENTATION_VIEW_CYLINDER_WIDGET__
#define __VTX_UI_WIDGET_REPRESENTATION_VIEW_CYLINDER_WIDGET__

#include "base_representation_view_widget.hpp"

namespace VTX::UI::Widget::Representation::View
{
	class CylinderWidget : public BaseRepresentationViewWidget
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
		void onRadiusChange( const float p_newRadius );
		void onColorBlendingModeChange( const Generic::COLOR_BLENDING_MODE & p_newMode );

	  protected:
		CylinderWidget( QWidget * p_parent = nullptr ) : BaseRepresentationViewWidget( p_parent ) {}
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QLabel * _radiusLabel			 = nullptr;
		QLabel * _colorBlendingModeLabel = nullptr;

		CustomWidget::FloatFieldSliderWidget * _radiusWidget			= nullptr;
		CustomWidget::QComboBoxMultiField *	   _colorBlendingModeWidget = nullptr;

		void _onRadiusChange( const float p_newRadius );
		void _onColorBlendingModeChange( const int p_newMode );
	};
} // namespace VTX::UI::Widget::Representation::View
#endif
