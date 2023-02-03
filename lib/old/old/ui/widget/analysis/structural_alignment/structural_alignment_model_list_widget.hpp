#ifndef __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT_MODEL_LIST_WIDGET__
#define __VTX_UI_WIDGET_ANALYSIS_STRUCTURAL_ALIGNMENT_MODEL_LIST_WIDGET__

#include "model/base_model.hpp"
#include "ui/widget/custom_widget/model_list_component.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>
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

		class ModelFieldLine : public QObject
		{
		  public:
			ModelFieldLine( StructuralAlignmentModelListWidget * const p_owner );
			~ModelFieldLine();

			void destroyWidgets();

			QRadioButton *					 getTickButton() const { return _tickButton; }
			QPushButton *					 getRemoveButton() const { return _removeButton; }
			QPushButton *					 getGrabButton() const { return _grabButton; }
			CustomWidget::ModelFieldWidget * getModelField() const { return _modelField; }
			QLabel *						 getRMSDLabel() const { return _rmsdLabel; }

			Model::BaseModel * const getModel() const { return _modelField->getModel(); }
			void					 setModel( Model::BaseModel * const p_model );

			bool isTicked() const;
			void setTickState( const bool p_ticked ) const;

			void setRMSD( const float p_rmsd, const size_t p_residueCount );
			void resetRMSD();

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
		void localize() override;

		void addModel( Model::BaseModel * const p_model ) override;
		void insertModel( Model::BaseModel * const p_model, const int p_row ) override;
		void removeModel( Model::BaseModel * const p_model ) override;
		void swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const;

		bool							hasModel( const Model::BaseModel * const p_model ) const override;
		int								getModelCount() const override;
		std::vector<Model::BaseModel *> getModels() const override;

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
			res.reserve( _lines.size() );

			for ( ModelFieldLine * const line : _lines )
				if ( !line->isTicked() )
					res.emplace_back( static_cast<T *>( line->getModel() ) );

			res.shrink_to_fit();

			return res;
		}

		void setRMSD( const Model::BaseModel * const p_model, const float p_rmsd, const size_t p_residueCount ) const;
		void resetRMSD( const Model::BaseModel * const p_model ) const;

	  protected:
		StructuralAlignmentModelListWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QGridLayout *				  _layout = nullptr;
		std::vector<ModelFieldLine *> _lines  = std::vector<ModelFieldLine *>();

		void _initColumn( const GRID_LAYOUT_COLUMN p_column, const std::string p_title, const int p_stretch );
		void _addModelInLayout( Model::BaseModel * const p_model, const int p_row );
		bool _isModelAlreadyInList( const Model::BaseModel * const p_model ) const;
		ModelFieldLine * _findLineFromModel( const Model::BaseModel * const p_model ) const;
		ModelFieldLine * _findNextLine( const ModelFieldLine * const p_from ) const;
	};
} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
#endif
