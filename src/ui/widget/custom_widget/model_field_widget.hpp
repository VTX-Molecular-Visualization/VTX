#ifndef __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__
#define __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__

#include "id.hpp"
#include "ui/widget/custom_widget/model_drop_area.hpp"
#include <QLabel>
#include <QWidget>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
	}

	namespace UI::Widget::CustomWidget
	{
		class ModelFieldWidget : public CustomWidget::ModelDropArea
		{
			Q_OBJECT
			VTX_WIDGET

		  public:
			~ModelFieldWidget() {};
			void localize() override;

			void refresh();

		  signals:
			void onModelChanged( Model::BaseModel * const p_model );

		  protected:
			ModelFieldWidget( QWidget * p_parent ) : CustomWidget::ModelDropArea( p_parent ) {};
			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			void _onModelDropped();

		  private:
			QLabel * _label				  = nullptr;
			QLabel * _modelTypeIconWidget = nullptr;

			QString _placeholder = "Drag a model here";
		};
	} // namespace UI::Widget::CustomWidget
} // namespace VTX
#endif
