#ifndef __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT__
#define __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT__

#include "align_parameters_widget.hpp"
#include "generic/base_scene_item.hpp"
#include "ui/layout/attribute_list_layout.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include "ui/widget/custom_widget/model_field_list_widget.hpp"
#include "ui/widget/custom_widget/model_field_widget.hpp"
#include <QDockWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
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

		  public:
			void receiveEvent( const Event::VTXEvent & p_event ) override;
			void localize() override;

			void refresh();

		  protected:
			StructuralAlignmentWidget( QWidget * p_parent );
			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			virtual void showEvent( QShowEvent * p_event ) override;

			void _updateTargetedMoleculesWithSelection( const Model::Selection & p_selection );
			void _computeAlign() const;

			void _onModelListChange();

		  private:
			CustomWidget::ModelFieldListWidget * _moleculesField			= nullptr;
			AlignParametersWidget *				 _alignmentParametersWidget = nullptr;

			QPushButton * _alignButton = nullptr;

			Model::Molecule *			   _staticMolecule	= nullptr;
			std::vector<Model::Molecule *> _mobileMolecules = std::vector<Model::Molecule *>();
		};

	} // namespace UI::Widget::Analysis::StructuralAlignment
} // namespace VTX

#endif
