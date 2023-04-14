#ifndef __VTX_UI_WIDGET_REPRESENTATION_INSTANTIATED_REPRESENTATION_LIST_WIDGET__
#define __VTX_UI_WIDGET_REPRESENTATION_INSTANTIATED_REPRESENTATION_LIST_WIDGET__

#include "old_ui/ui/widget/custom_widget/model_list_component.hpp"
#include "old_ui/ui/widget/custom_widget/model_list_widget.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>
#include <app/old_app/model/base_model.hpp>
#include <app/old_app/model/representation/instantiated_representation.hpp>
#include <vector>

namespace VTX::UI::Widget::Representation
{
	class InstantiatedRepresentationListWidget : public CustomWidget::BaseModelListWidget
	{
		VTX_WIDGET

	  private:
		enum class GRID_LAYOUT_COLUMN : int
		{
			MODEL		  = 0,
			REMOVE_BUTTON = 1,
		};

		class ModelFieldLine : public CustomWidget::BaseModelFieldLine
		{
		  public:
			ModelFieldLine( InstantiatedRepresentationListWidget * const p_owner );
			~ModelFieldLine();

			QPushButton * getRemoveButton() const { return _removeButton; }
			QPushButton * getModelButton() const { return _modelButton; }

			virtual Model::BaseModel * const getModel() const { return _instantiatedRepresentation; }
			void							 setModel( Model::BaseModel * const p_model ) override;

		  protected:
			void _onModelButtonClicked();
			void _callRemoveAction();

		  private:
			InstantiatedRepresentationListWidget * const		_owner						= nullptr;
			Model::Representation::InstantiatedRepresentation * _instantiatedRepresentation = nullptr;

			QPushButton * _modelButton	= nullptr;
			QPushButton * _removeButton = nullptr;
		};

	  protected:
		InstantiatedRepresentationListWidget( QWidget * p_parent = nullptr );

		void _initColumns() override;

		CustomWidget::BaseModelFieldLine * const _instantiateLine();
		void _addLineInLayout( CustomWidget::BaseModelFieldLine * const p_line, const int p_row );
	};
} // namespace VTX::UI::Widget::Representation
#endif
