#ifndef __VTX_UI_WIDGET_REPRESENTATION_PRESET_EDITOR__
#define __VTX_UI_WIDGET_REPRESENTATION_PRESET_EDITOR__

#include "ui/old_ui/ui/layout/attribute_list_layout.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/color_field_button.hpp"
#include "ui/old_ui/ui/widget/custom_widget/filename_field_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/old_ui/view/ui/editor_view.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <app/old/application/representation/enum_representation.hpp>
#include <app/old/application/representation/representation_preset.hpp>
#include <util/color/rgba.hpp>
#include <type_traits>
#include <vector>

namespace VTX::UI::Widget::Settings
{
	class RepresentationPresetEditor :
		public VTX::UI::Widget::BaseManualWidget<QScrollArea>,
		View::UI::EditorView<App::Old::Application::Representation::RepresentationPreset>
	{
		VTX_WIDGET

	  private:
		using Representation = App::Old::Application::Representation::RepresentationPreset;

		enum class OPTIONAL_PARAMETER : int
		{
			SPHERE_RADIUS,
			CYLINDER_RADIUS,
			CYLINDER_COLOR_BLENDING_MODE,
			COLOR_MODE,
			RIBBON_COLOR_MODE,
			RIBBON_COLOR_BLENDING_MODE,

			COUNT,
		};

		static const std::vector<std::vector<OPTIONAL_PARAMETER>> _MAP_REPRESENTATION_PARAMETERS;

	  public:
		void localize() override;
		void refresh();

		const App::Old::Application::Representation::RepresentationPreset * const getPreset() const { return _preset; };
		void setPreset( App::Old::Application::Representation::RepresentationPreset * const p_model,
						const bool													   p_updateRender = true );

	  protected:
		RepresentationPresetEditor( QWidget * const p_parent );
		~RepresentationPresetEditor();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;
		void _catchModelEvent( const VTX::App::Old::Core::Event::VTXEvent * const p_event ) override;

	  private:
		Representation * _preset = nullptr;

		Layout::AttributeListLayout * _attributeLayout = nullptr;

		CustomWidget::FilenameFieldWidget * _nameWidget				  = nullptr;
		QCheckBox *							_quickAccess			  = nullptr;
		QComboBox *							_representationTypeWidget = nullptr;

		CustomWidget::FloatFieldSliderWidget * _sphereRadiusWidget		  = nullptr;
		CustomWidget::FloatFieldSliderWidget * _cylinderRadiusWidget	  = nullptr;
		QComboBox *							   _cylinderColorBlendingMode = nullptr;
		QComboBox *							   _colorModeWidget			  = nullptr;
		QComboBox *							   _ribbonColorModeWidget	  = nullptr;
		QComboBox *							   _ribbonColorBlendingMode	  = nullptr;
		CustomWidget::ColorFieldButton *	   _colorButtonWidget		  = nullptr;

		QPushButton * _setDefaultButton = nullptr;

		std::vector<QWidget *> _optionalParameterWidgets = std::vector<QWidget *>();

		void _refreshRepresentationData();
		void _refreshBallAndStickRepresentation();
		void _refreshBallStickAndCartoonRepresentation();
		void _refreshStickRepresentation();
		void _refreshStickAndCartoonRepresentation();
		void _refreshVanDerWaalsRepresentation();
		void _refreshTraceRepresentation();
		void _refreshSASRepresentation();
		void _refreshCartoonRepresentation();
		void _refreshSESRepresentation();

		void _refreshOptionalParametersVisibility(
			const App::Old::Application::Representation::REPRESENTATION_ENUM & p_representationEnum );

		void _onRepresentationNameChange();
		void _onQuickAccessChange( const int p_state );
		void _onRepresentationTypeChange( const int p_newIndex );
		void _onSphereRadiusChanged( const float p_value );
		void _onCylinderRadiusChanged( const float p_value );
		void _onCylinderColorBendingModeChanged( const int p_index );
		void _onRibbonColorModeChanged( const int p_index );
		void _onRibbonColorBlendingModeChanged( const int p_index );
		void _onColorModeChanged( const int p_index );
		void _onColorChanged( const Util::Color::Rgba & p_color );
		void _onSetDefault();
	};

} // namespace VTX::UI::Widget::Settings

#endif
