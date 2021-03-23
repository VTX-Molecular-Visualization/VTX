#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_RESIDUE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/residue.hpp"
#include "ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/inspector/inspector_item_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/representation/representation_inspector_section.hpp"
#include "view/base_view.hpp"
#include <QWidget>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	class MultipleResidueWidget : public InspectorItemWidget
	{
		VTX_WIDGET

	  public:
		~MultipleResidueWidget();

		void localize() override;

		void clearTargets();
		void addTarget( Model::Residue * const p_target, const bool p_refresh = true );
		void removeTarget( Model::Residue * const p_target, const bool p_refresh = true );

		void refresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  protected:
		MultipleResidueWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

	  private:
		std::unordered_set<Model::Residue *> _targets = std::unordered_set<Model::Residue *>();

		InspectorSection *		  _representationSection = nullptr;
		InspectorSectionVLayout * _infoSection			 = nullptr;

		Representation::RepresentationInspectorSection * _representationWidget;

		CustomWidget::QLabelMultiField * _fullnameLabel = nullptr;
		CustomWidget::QLabelMultiField * _nbAtomsLabel	= nullptr;

		void _onRepresentationPresetChange( const int p_presetIndex );
		void _onRepresentationChange( const Model::Representation::InstantiatedRepresentation & p_representation,
									  const Model::Representation::MEMBER_FLAG &				p_flag );
		void _onRepresentationColorChange( const Model::Representation::InstantiatedRepresentation & p_representation,
										   const Color::Rgb &										 p_color,
										   const bool												 p_ssColor );

		void _resetFieldStates( const SectionFlag & p_flag );
		void _changeMoleculesColor( const Color::Rgb & p_color ) const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif
