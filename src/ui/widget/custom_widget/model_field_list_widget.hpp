#ifndef __VTX_UI_WIDGET_MODEL_FIELD_LIST_WIDGET__
#define __VTX_UI_WIDGET_MODEL_FIELD_LIST_WIDGET__

#include "model/base_model.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/folding_button.hpp"
#include "ui/widget/custom_widget/model_drop_area.hpp"
#include "ui/widget/custom_widget/model_field_widget.hpp"
#include "ui/widget/custom_widget/textual_model_drop_area.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>
#include <vector>

namespace VTX::UI::Widget::CustomWidget
{
	class ModelFieldListWidget : public BaseManualWidget<QWidget>
	{
		Q_OBJECT
		VTX_WIDGET

	  private:
		enum class GRID_LAYOUT_COLUMN : int
		{
			TICK		  = 0,
			MODEL		  = 1,
			REMOVE_BUTTON = 2
		};

		class ModelFieldLine : public QObject
		{
		  public:
			ModelFieldLine( ModelFieldListWidget * const p_owner );
			~ModelFieldLine();

			void destroyWidgets();

			QRadioButton *					 getTickButton() const { return _tickButton; }
			QPushButton *					 getRemoveButton() const { return _removeButton; }
			QPushButton *					 getGrabButton() const { return _grabButton; }
			CustomWidget::ModelFieldWidget * getModelField() const { return _modelField; }

			Model::BaseModel * const getModel() const { return _modelField->getModel(); }
			void					 setModel( Model::BaseModel * const p_model );

			bool isTicked() const;
			void setTickState( const bool p_ticked );

		  protected:
			void _callRemoveAction();
			void _checkModelChange( Model::BaseModel * const p_model );

		  private:
			ModelFieldListWidget * const _owner = nullptr;

			QRadioButton *					 _tickButton   = nullptr;
			QPushButton *					 _removeButton = nullptr;
			QPushButton *					 _grabButton   = nullptr;
			CustomWidget::ModelFieldWidget * _modelField   = nullptr;
		};

	  public:
		~ModelFieldListWidget();
		void localize() override;

		bool getContainsOnlyUniqueModel() const { return _containsOnlyUniqueModel; }
		void setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel );

		void addModel( Model::BaseModel * const p_model );
		void insertModel( Model::BaseModel * const p_model, const int p_position );
		void removeModel( Model::BaseModel * const p_model );

		bool							hasModel( const Model::BaseModel * const p_model ) const;
		int								getModelCount() const;
		std::vector<Model::BaseModel *> getModels() const;

		void swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const;

		Model::BaseModel * const getTickedModel() const;
		template<typename T>
		T * const getTickedModel() const
		{
			return static_cast<T *>( getTickedModel() );
		}

		std::vector<Model::BaseModel *> const getNotTickedModels() const;

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

		template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
		std::vector<M *> getModels() const
		{
			std::vector<M *> res = std::vector<M *>();
			res.reserve( _lines.size() );

			for ( const ModelFieldLine * const line : _lines )
				res.emplace_back( static_cast<M *>( line->getModel() ) );

			return res;
		}

		void setTitle( const QString & p_title ) const;

	  signals:
		void onModelListChange();

	  protected:
		ModelFieldListWidget( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void			 _addModelInLayout( Model::BaseModel * const p_model, const int row );
		bool			 _isModelAlreadyInList( Model::BaseModel * const p_model ) const;
		ModelFieldLine * _findLineFromModel( Model::BaseModel * const p_model ) const;
		ModelFieldLine * _findNextLine( const ModelFieldLine * const p_from ) const;

		void _onModelsDropped( std::vector<Model::BaseModel *> p_models );

	  private:
		CustomWidget::FoldingButton *		 _foldButton	  = nullptr;
		QGridLayout *						 _modelListLayout = nullptr;
		CustomWidget::TextualModelDropArea * _dropArea		  = nullptr;

		std::vector<ModelFieldLine *> _lines				   = std::vector<ModelFieldLine *>();
		bool						  _containsOnlyUniqueModel = true;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
