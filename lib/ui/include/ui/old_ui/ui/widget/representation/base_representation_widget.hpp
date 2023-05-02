#ifndef __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__

#include "ui/old_ui/ui/multi_data_field.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/color_mode_field_widget.hpp"
#include "view/cylinder_widget.hpp"
#include "view/ribbon_widget.hpp"
#include "view/sphere_widget.hpp"
#include <QWidget>
#include <app/application/representation/instantiated_representation.hpp>
#include <unordered_set>

namespace VTX::View::UI::Widget::Representation
{
	class InstantiatedRepresentationView;
}

namespace VTX::UI::Widget::Representation
{
	class BaseRepresentationWidget :
		public VTX::UI::Widget::BaseManualWidget<QWidget>,
		public TMultiDataField<const App::Application::Representation::InstantiatedRepresentation>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		using InstantiatedRepresentation	 = App::Application::Representation::InstantiatedRepresentation;
		using InstantiatedRepresentationView = VTX::View::UI::Widget::Representation::InstantiatedRepresentationView;
		using MEMBER_FLAG					 = App::Application::Representation::MEMBER_FLAG;

		void refresh();
		void setRepresentation( InstantiatedRepresentation * const p_representation );

		// MultiDataField Implementation //////////////////////////////
		void updateWithNewValue(
			const App::Application::Representation::InstantiatedRepresentation & p_value ) override;
		void resetState() override;
		//////////////////////////////////////////////////////////////

	  signals:
		void onDataChange( const App::Application::Representation::MEMBER_FLAG & p_flag );
		void onColorChange( const Util::Color::Rgba & p_color, const bool p_ssColor );

	  protected:
		BaseRepresentationWidget( QWidget * p_parent = nullptr );

		InstantiatedRepresentation * _instantiatedRepresentation = nullptr;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		// MultiDataField Implementation //////////////////////////////
		void _displayDifferentsDataFeedback() override;
		//////////////////////////////////////////////////////////////

		void _addSphereWidgetInLayout();
		void _addCylinderWidgetInLayout();
		void _addRibbonWidgetInLayout();
		void _addColorModeWidgetInLayout();

		void _refreshColorModeWidget();
		void _addColorModeValue( const InstantiatedRepresentation & p_representation );

		virtual void _onSphereRadiusChange( const float p_newRadius );
		virtual void _onSphereRadiusOffsetChange( const float p_newRadius );
		virtual void _onCylinderRadiusChange( const float p_newRadius );
		virtual void _onCylinderColorBlendingModeChange( const Generic::COLOR_BLENDING_MODE & p_colorBlendindrMode );
		virtual void _onRibbonColorChange( const Util::Color::Rgba & p_color );
		virtual void _onRibbonColorModeChange( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode );
		virtual void _onRibbonColorBlendingModeChange( const Generic::COLOR_BLENDING_MODE & p_colorBlendindrMode );
		virtual void _onColorChange( const Util::Color::Rgba & p_color );
		virtual void _onColorModeChange( const Generic::COLOR_MODE & p_colorMode );

		Representation::View::SphereWidget *   _sphereWidget   = nullptr;
		Representation::View::CylinderWidget * _cylinderWidget = nullptr;
		Representation::View::RibbonWidget *   _ribbonWidget   = nullptr;

		QLabel *							 _colorModeLabel  = nullptr;
		CustomWidget::ColorModeFieldWidget * _colorModeWidget = nullptr;

	  private:
		QGridLayout *																	_layout = nullptr;
		std::unordered_set<const App::Application::Representation::BaseRepresentable *> _targets
			= std::unordered_set<const App::Application::Representation::BaseRepresentable *>();

		void _appendWidgetInLayout( QWidget * const p_label, QWidget * const p_widget );
	};
} // namespace VTX::UI::Widget::Representation
#endif
