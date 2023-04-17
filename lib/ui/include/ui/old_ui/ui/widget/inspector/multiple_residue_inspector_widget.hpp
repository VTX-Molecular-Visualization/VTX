#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_RESIDUE__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_RESIDUE__

#include "ui/old_ui/ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/old_ui/ui/layout/attribute_list_layout.hpp"
#include "ui/old_ui/ui/widget/inspector/inspector_section.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include "ui/old_ui/ui/widget/representation/representation_inspector_section.hpp"
#include <QLabel>
#include <QWidget>
#include <app/old_app/model/representation/instantiated_representation.hpp>
#include <app/old_app/model/representation/representation.hpp>
#include <app/old_app/model/residue.hpp>
#include <app/old_app/view/base_view.hpp>

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

		void _setInspectorToMolecule() const;
		void _setInspectorToChain() const;
		void _setInspectorToAtom() const;

	  private:
		InspectorSection * _representationSection = nullptr;
		InspectorSection * _infoSection			  = nullptr;

		Representation::RepresentationInspectorSection * _representationWidget;

		Layout::AttributeListLayout * _infoSectionLayout = nullptr;
		QLabel *					  _fullnameLabel	 = nullptr;
		QLabel *					  _atomsCountLabel	 = nullptr;
		QLabel *					  _atomsSumLabel	 = nullptr;
		QLabel *					  _bondsLabel		 = nullptr;

		int _bondInfoCount = 0;

		void _onRepresentationPresetChange( const int p_presetIndex );
		void _onRepresentationChange( const Model::Representation::InstantiatedRepresentation & p_representation,
									  const Model::Representation::MEMBER_FLAG &				p_flag );
		void _onRepresentationColorChange( const Model::Representation::InstantiatedRepresentation & p_representation,
										   const Color::Rgba &										 p_color,
										   const bool												 p_ssColor );
		void _onRevertRepresentation() const;

		void _resetFieldStates( const SectionFlag & p_flag );
		void _changeMoleculesColor( const Color::Rgba & p_color ) const;
		void _appendBondInfo( const Model::Residue & p_residue );
	};
} // namespace VTX::UI::Widget::Inspector

#endif
