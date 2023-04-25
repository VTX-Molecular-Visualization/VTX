#ifndef __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT__
#define __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT__

#include "align_parameters_widget.hpp"
#include "tool/old_tool/ui/widget/analysis/structural_alignment/structural_alignment_model_list_widget.hpp"
#include <QDockWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <app/old_app/generic/base_scene_item.hpp>
#include <ui/old_ui/ui/layout/attribute_list_layout.hpp>
#include <ui/old_ui/ui/widget/base_manual_widget.hpp>
#include <ui/old_ui/ui/widget/custom_widget/dock_window_main_widget.hpp>
#include <ui/old_ui/ui/widget/custom_widget/model_list_component.hpp>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
		class Molecule;
		class Selection;
	} // namespace Model

	namespace UI::Widget::Analysis::StructuralAlignment
	{
		class StructuralAlignmentWidget : public BaseManualWidget<QDockWidget>
		{
			VTX_WIDGET

			using StructuralAlignmentModelListWidget
				= UI::Widget::Analysis::StructuralAlignment::StructuralAlignmentModelListWidget;

		  public:
			void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;
			void localize() override;

			void refresh();

		  protected:
			StructuralAlignmentWidget( QWidget * p_parent );
			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			virtual void showEvent( QShowEvent * p_event ) override;

			void _updateTargetedMoleculesWithSelection( const Model::Selection & p_selection );
			void _computeAlign() const;
			bool _isConcernedByAlignment( const VTX::Analysis::StructuralAlignment::AlignmentResult & p_result ) const;

			void _onModelListChange();

		  private:
			StructuralAlignmentModelListWidget * _moleculeList				= nullptr;
			CustomWidget::ModelListComponent *	 _moleculesComponent		= nullptr;
			AlignParametersWidget *				 _alignmentParametersWidget = nullptr;

			QPushButton * _alignButton = nullptr;
		};

	} // namespace UI::Widget::Analysis::StructuralAlignment
} // namespace VTX

#endif
