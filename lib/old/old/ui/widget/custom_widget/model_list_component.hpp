#ifndef __VTX_UI_WIDGET_MODEL_LIST_COMPONENT__
#define __VTX_UI_WIDGET_MODEL_LIST_COMPONENT__

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
	class BaseModelListWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		BaseModelListWidget( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent ) {};

		virtual void addModel( Model::BaseModel * const p_model )											  = 0;
		virtual void insertModel( Model::BaseModel * const p_model, const int p_row )						  = 0;
		virtual void removeModel( Model::BaseModel * const p_model )										  = 0;
		virtual void swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const = 0;

		virtual bool							hasModel( const Model::BaseModel * const p_model ) const = 0;
		virtual int								getModelCount() const									 = 0;
		virtual std::vector<Model::BaseModel *> getModels() const										 = 0;

		template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
		std::vector<M *> getModels() const
		{
			std::vector<M *>					  res		 = std::vector<M *>();
			const std::vector<Model::BaseModel *> baseModels = getModels();
			res.reserve( baseModels.size() );

			for ( const Model::BaseModel * const model : baseModels )
				res.emplace_back( static_cast<M *>( model ) );

			return res;
		}

		bool		 getContainsOnlyUniqueModel() const { return _containsOnlyUniqueModel; }
		virtual void setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel );

		const std::vector<ID::VTX_ID> & getFilters() const { return _filters; }
		virtual void					addTypeFilter( const ID::VTX_ID & p_typeID );

	  signals:
		void onModelListChange();

	  private:
		bool					_containsOnlyUniqueModel = true;
		std::vector<ID::VTX_ID> _filters				 = std::vector<ID::VTX_ID>();
	};

	class ModelListComponent : public BaseManualWidget<QWidget>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~ModelListComponent();
		void receiveEvent( const Event::VTXEvent & p_event ) override;

		void localize() override;

		void setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel ) const;
		void addTypeFilter( const ID::VTX_ID & p_typeID ) const;

		void setTitle( const QString & p_title ) const;
		void setFoldState( const bool p_expanded ) const;

		void addModel( Model::BaseModel * const p_model );
		void insertModel( Model::BaseModel * const p_model, const int p_row );
		void removeModel( Model::BaseModel * const p_model );
		void swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const;

		bool							hasModel( const Model::BaseModel * const p_model ) const;
		int								getModelCount() const;
		std::vector<Model::BaseModel *> getModels() const;

		template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
		std::vector<M *> getModels() const
		{
			return _modelListWidget->getModels<M>();
		}

		const BaseModelListWidget * getModelListWidget() const { return _modelListWidget; }

	  signals:
		void onModelListChange();

	  protected:
		ModelListComponent( BaseModelListWidget * const p_modelList, QWidget * p_parent = nullptr );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _emitModelListChangeEvent();
		void _onModelsDropped( std::vector<Model::BaseModel *> p_models );

	  private:
		CustomWidget::FoldingButton *		 _foldButton	  = nullptr;
		BaseModelListWidget *				 _modelListWidget = nullptr;
		CustomWidget::TextualModelDropArea * _dropArea		  = nullptr;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
