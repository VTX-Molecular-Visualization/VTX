#ifndef __VTX_UI_WIDGET_REPRESENTATION_VIEW_SPHERE_WIDGET__
#define __VTX_UI_WIDGET_REPRESENTATION_VIEW_SPHERE_WIDGET__

#include "base_representation_view_widget.hpp"

namespace VTX::UI::Widget::Representation::View
{
	class SphereWidget : public BaseRepresentationViewWidget
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		void refresh( const App::Old::Application::Representation::InstantiatedRepresentation &		 p_representation,
					  const std::unordered_set<const App::Old::Application::Representation::BaseRepresentable *> & p_targets ) override;
		void updateWithNewValue( const App::Old::Application::Representation::InstantiatedRepresentation &		p_representation,
								 const std::unordered_set<const App::Old::Application::Representation::BaseRepresentable *> & p_targets ) override;
		void resetState() override;
		void setRadiusWidgetVisible( const bool );
		void setRadiusOffsetWidgetVisible( const bool );

	  signals:
		void onRadiusChange( const float p_newRadius );
		void onRadiusOffsetChange( const float p_newRadius );

	  protected:
		SphereWidget( QWidget * p_parent = nullptr ) : BaseRepresentationViewWidget( p_parent ) {}
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QLabel * _radiusLabel		= nullptr;
		QLabel * _radiusOffsetLabel = nullptr;

		CustomWidget::FloatFieldSliderWidget * _radiusWidget	   = nullptr;
		CustomWidget::FloatFieldSliderWidget * _radiusOffsetWidget = nullptr;

		void _onRadiusChange( const float p_newRadius );
		void _onRadiusOffsetChange( const float p_newRadius );
	};
} // namespace VTX::UI::Widget::Representation::View
#endif
