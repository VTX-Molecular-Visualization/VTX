#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MOLECULE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/custom_widget/transform_widget.hpp"
#include "ui/widget/inspector/inspector_item_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/representation/representation_inspector_section.hpp"
#include "view/base_view.hpp"
#include <QWidget>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	class MultipleMoleculeWidget : public InspectorItemWidget
	{
		VTX_WIDGET

	  public:
		~MultipleMoleculeWidget();

		void localize() override;

		void clearTargets();
		void addTarget( Model::Molecule * const p_target, const bool p_refresh = true );
		void removeTarget( Model::Molecule * const p_target, const bool p_refresh = true );

		void refresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  protected:
		MultipleMoleculeWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

	  private:
		std::unordered_set<Model::Molecule *> _targets = std::unordered_set<Model::Molecule *>();

		InspectorSection *		  _transformSection		 = nullptr;
		InspectorSection *		  _representationSection = nullptr;
		InspectorSectionVLayout * _infoSection			 = nullptr;

		CustomWidget::TransformWidget * _transformWidget = nullptr;

		Representation::RepresentationInspectorSection * _representationWidget;

		CustomWidget::QLabelMultiField * _fullnameLabel	  = nullptr;
		CustomWidget::QLabelMultiField * _nbChainsLabel	  = nullptr;
		CustomWidget::QLabelMultiField * _nbResiduesLabel = nullptr;
		CustomWidget::QLabelMultiField * _nbAtomsLabel	  = nullptr;

		void _resetFieldStates( const SectionFlag & p_flag );

		void _onTransformChange( const Math::Transform & ) const;
		void _onRepresentationPresetChange( const int p_presetIndex ) const;
		void _onRepresentationChange( const Model::Representation::InstantiatedRepresentation & p_representation,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) const;
		void _onRepresentationColorChange( const Model::Representation::InstantiatedRepresentation & p_representation,
										   const Color::Rgb &										 p_color,
										   const bool												 p_ssColor ) const;
		void _onRevertRepresentation() const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif
