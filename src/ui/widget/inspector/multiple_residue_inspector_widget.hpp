#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_RESIDUE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_RESIDUE__

#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/residue.hpp"
#include "ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include "ui/widget/representation/representation_inspector_section.hpp"
#include "view/base_view.hpp"
#include <QLabel>
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	class MultipleResidueWidget : public MultipleModelInspectorWidget<Model::Residue>
	{
		VTX_WIDGET

	  public:
		~MultipleResidueWidget();
		void localize() override;

	  protected:
		MultipleResidueWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  private:
		InspectorSection *		  _representationSection = nullptr;
		InspectorSectionVLayout * _infoSection			 = nullptr;

		Representation::RepresentationInspectorSection * _representationWidget;

		CustomWidget::QLabelMultiField * _fullnameLabel = nullptr;
		CustomWidget::QLabelMultiField * _nbAtomsLabel	= nullptr;
		QLabel *						 _bondsLabel	= nullptr;

		void _onRepresentationPresetChange( const int p_presetIndex );
		void _onRepresentationChange( const Model::Representation::InstantiatedRepresentation & p_representation,
									  const Model::Representation::MEMBER_FLAG &				p_flag );
		void _onRepresentationColorChange( const Model::Representation::InstantiatedRepresentation & p_representation,
										   const Color::Rgb &										 p_color,
										   const bool												 p_ssColor );
		void _onRevertRepresentation() const;

		void _resetFieldStates( const SectionFlag & p_flag );
		void _changeMoleculesColor( const Color::Rgb & p_color ) const;
		void _appendBondInfo( const Model::Residue & p_residue );
	};
} // namespace VTX::UI::Widget::Inspector

#endif
