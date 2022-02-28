#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MOLECULE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MOLECULE__

#include "color/rgb.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/custom_widget/auto_rotate_widget.hpp"
#include "ui/widget/custom_widget/color_field_button.hpp"
#include "ui/widget/custom_widget/trajectory_widget.hpp"
#include "ui/widget/custom_widget/transform_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include "ui/widget/representation/representation_inspector_section.hpp"
#include "view/base_view.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	class MultipleMoleculeWidget : public MultipleModelInspectorWidget<Model::Molecule>
	{
		VTX_WIDGET

	  public:
		~MultipleMoleculeWidget();
		void localize() override;

	  protected:
		MultipleMoleculeWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _resetFieldStates( const SectionFlag & p_flag ) override;
		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  private:
		InspectorSection *		  _transformSection		 = nullptr;
		InspectorSectionVLayout * _representationSection = nullptr;
		InspectorSection *		  _trajectorySection	 = nullptr;
		InspectorSection *		  _autoRotateSection	 = nullptr;
		InspectorSectionVLayout * _infoSection			 = nullptr;

		CustomWidget::TransformWidget * _transformWidget = nullptr;

		Representation::RepresentationInspectorSection * _representationWidget = nullptr;
		CustomWidget::ColorFieldButton *				 _moleculeColor		   = nullptr;

		CustomWidget::TrajectoryWidget * _trajectoryWidget = nullptr;
		CustomWidget::AutoRotateWidget * _autoRotateField  = nullptr;

		CustomWidget::QLabelMultiField * _fullnameLabel	  = nullptr;
		CustomWidget::QLabelMultiField * _nbChainsLabel	  = nullptr;
		CustomWidget::QLabelMultiField * _nbResiduesLabel = nullptr;
		CustomWidget::QLabelMultiField * _nbAtomsLabel	  = nullptr;

		void _onTransformChange( const Math::Transform & ) const;
		void _onPositionDragged( const Vec3f & ) const;
		void _onRotationDragged( const Vec3f & ) const;
		void _onScaleDragged( const Vec3f & ) const;

		void _onRepresentationPresetChange( const int p_presetIndex ) const;
		void _onRepresentationChange( const Model::Representation::InstantiatedRepresentation & p_representation,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) const;
		void _onRepresentationColorChange( const Model::Representation::InstantiatedRepresentation & p_representation,
										   const Color::Rgb &										 p_color,
										   const bool												 p_ssColor ) const;
		void _onRevertRepresentation() const;
		void _onApplyRepresentationToChildren() const;
		void _onMoleculeColorChange( const Color::Rgb & p_color ) const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif
