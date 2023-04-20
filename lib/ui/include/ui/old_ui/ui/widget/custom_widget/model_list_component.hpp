#ifndef __VTX_UI_WIDGET_MODEL_LIST_COMPONENT__
#define __VTX_UI_WIDGET_MODEL_LIST_COMPONENT__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/folding_button.hpp"
#include "ui/old_ui/ui/widget/custom_widget/model_drop_area.hpp"
#include "ui/old_ui/ui/widget/custom_widget/model_field_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/model_list_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/textual_model_drop_area.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>
#include <app/model/base_model.hpp>
#include <vector>

namespace VTX::UI::Widget::CustomWidget
{
	class ModelListComponent : public BaseManualWidget<QWidget>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~ModelListComponent();
		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

		void localize() override;

		void setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel ) const;
		void addTypeFilter( const ID::VTX_ID & p_typeID ) const;

		void setTitle( const QString & p_title ) const;
		void setFoldState( const bool p_expanded ) const;

		void addModel( Model::BaseModel * const p_model );
		void insertModel( Model::BaseModel * const p_model, const int p_row );
		void removeModel( Model::BaseModel * const p_model );
		void swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const;
		void clearModels();

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
		CustomWidget::BaseModelListWidget *	 _modelListWidget = nullptr;
		CustomWidget::TextualModelDropArea * _dropArea		  = nullptr;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
