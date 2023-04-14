#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_CHAIN__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_CHAIN__

#include "old_ui/ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "old_ui/ui/layout/attribute_list_layout.hpp"
#include "old_ui/ui/multi_data_field.hpp"
#include "old_ui/ui/widget/custom_widget/folding_button.hpp"
#include "old_ui/ui/widget/custom_widget/model_list_component.hpp"
#include "old_ui/ui/widget/inspector/inspector_section.hpp"
#include "old_ui/ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include "old_ui/ui/widget/representation/instantiated_representation_list_widget.hpp"
#include "old_ui/ui/widget/representation/representation_inspector_section.hpp"
#include <QWidget>
#include <app/old_app/model/chain.hpp>
#include <app/old_app/model/representation/instantiated_representation.hpp>
#include <app/old_app/model/representation/representation.hpp>

namespace VTX::UI::Widget::Inspector
{
	class MultipleChainWidget : public MultipleModelInspectorWidget<Model::Chain>
	{
		VTX_WIDGET

	  public:
		~MultipleChainWidget();
		void localize() override;

	  protected:
		MultipleChainWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _resetFieldStates( const SectionFlag & p_flag ) override;
		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

		void _setInspectorToMolecule() const;
		void _setInspectorToResidue() const;
		void _setInspectorToAtom() const;

	  private:
		InspectorSectionVLayout * _representationSection = nullptr;
		InspectorSection *		  _infoSection			 = nullptr;

		Representation::RepresentationInspectorSection *	   _representationWidget		   = nullptr;
		CustomWidget::FoldingButton *						   _subRepresentationFoldingButton = nullptr;
		Representation::InstantiatedRepresentationListWidget * _subRepresentationWidget		   = nullptr;

		Layout::AttributeListLayout * _infoSectionlayout = nullptr;

		QLabel * _fullnameLabel		 = nullptr;
		QLabel * _residuesCountLabel = nullptr;
		QLabel * _residuesSumLabel	 = nullptr;
		QLabel * _atomsCountLabel	 = nullptr;
		QLabel * _atomsSumLabel		 = nullptr;
		QLabel * _indexLabel		 = nullptr;

		int _subRepresentationPresentationCount = 0;

		void _onRepresentationPresetChange( const int p_presetIndex );
		void _onRepresentationChange( const Model::Representation::InstantiatedRepresentation & p_representation,
									  const Model::Representation::MEMBER_FLAG &				p_flag );
		void _onRepresentationColorChange( const Model::Representation::InstantiatedRepresentation & p_representation,
										   const Color::Rgba &										 p_color,
										   const bool												 p_ssColor );
		void _onRevertRepresentation() const;
		void _onApplyRepresentationToChildren() const;

		void _changeMoleculesColor( const Color::Rgba & p_color ) const;
	};
} // namespace VTX::UI::Widget::Inspector

#endif
