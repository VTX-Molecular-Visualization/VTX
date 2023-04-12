#ifndef __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT_MODEL_LIST_WIDGET__
#define __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT_MODEL_LIST_WIDGET__

#include "old_ui/ui/widget/custom_widget/model_list_component.hpp"
#include "old_ui/ui/widget/custom_widget/model_list_widget.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>
#include <old/model/base_model.hpp>
#include <vector>

namespace VTX::UI::Widget::Analysis::StructuralAlignment
{
	class StructuralAlignmentModelListWidget : public CustomWidget::BaseModelListWidget
	{
		VTX_WIDGET

	  private:
		enum class GRID_LAYOUT_COLUMN : int
		{
			TICK		  = 0,
			MODEL		  = 1,
			RMSD		  = 2,
			REMOVE_BUTTON = 3,
		};

		class ModelFieldLine : public CustomWidget::BaseModelFieldLine
		{
		  public:
			ModelFieldLine( StructuralAlignmentModelListWidget * const p_owner );
			~ModelFieldLine();

			QRadioButton *					 getTickButton() const { return _tickButton; }
			QPushButton *					 getRemoveButton() const { return _removeButton; }
			QPushButton *					 getGrabButton() const { return _grabButton; }
			CustomWidget::ModelFieldWidget * getModelField() const { return _modelField; }
			QLabel *						 getRMSDLabel() const { return _rmsdLabel; }

			virtual Model::BaseModel * const getModel() const { return _modelField->getModel(); }
			void							 setModel( Model::BaseModel * const p_model ) override;

			bool isTicked() const;
			void setTickState( const bool p_ticked ) const;

			void setRMSD( const float p_rmsd, const size_t p_residueCount );
			void resetRMSD();

			bool blockSignals( const bool p_block ) override;

		  protected:
			void _callRemoveAction();
			void _checkModelChange( Model::BaseModel * const p_model );

		  private:
			StructuralAlignmentModelListWidget * const _owner = nullptr;

			QRadioButton *					 _tickButton   = nullptr;
			QPushButton *					 _grabButton   = nullptr;
			CustomWidget::ModelFieldWidget * _modelField   = nullptr;
			QPushButton *					 _removeButton = nullptr;
			QLabel *						 _rmsdLabel	   = nullptr;
		};

	  public:
		Model::BaseModel * const getTickedModel() const;
		template<typename T>
		T * const getTickedModel() const
		{
			return static_cast<T *>( getTickedModel() );
		}

		std::vector<Model::BaseModel *> getNotTickedModels() const;
		template<typename T>
		std::vector<T *> const getNotTickedModels() const
		{
			std::vector<T *> res = std::vector<T *>();
			res.reserve( _getLines().size() );

			for ( CustomWidget::BaseModelFieldLine * const line : _getLines() )
			{
				ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( line );
				if ( !castedLine->isTicked() )
					res.emplace_back( static_cast<T *>( castedLine->getModel() ) );
			}

			res.shrink_to_fit();

			return res;
		}

		void setRMSD( const Model::BaseModel * const p_model, const float p_rmsd, const size_t p_residueCount ) const;
		void resetRMSD( const Model::BaseModel * const p_model ) const;

	  protected:
		StructuralAlignmentModelListWidget( QWidget * p_parent = nullptr );

		void _initColumns() override;

		CustomWidget::BaseModelFieldLine * const _instantiateLine();
		void _initLine( CustomWidget::BaseModelFieldLine * const p_line, Model::BaseModel * const p_model ) const;
		void _addLineInLayout( CustomWidget::BaseModelFieldLine * const p_line, const int p_row );

		void _onLineWillBeRemoved( CustomWidget::BaseModelFieldLine * const p_line ) override;
	};
} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
#endif
