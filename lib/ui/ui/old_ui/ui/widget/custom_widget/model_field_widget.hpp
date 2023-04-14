#ifndef __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__
#define __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__

#include "old_ui/ui/draggable_item.hpp"
#include "old_ui/ui/widget/custom_widget/model_drop_area.hpp"
#include <QLabel>
#include <QWidget>
#include <app/old_app/id.hpp>
#include <type_traits>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
	}

	namespace UI::Widget::CustomWidget
	{
		class ModelFieldWidget : public CustomWidget::ModelDropArea, public DraggableItem
		{
			Q_OBJECT
			VTX_WIDGET

		  public:
			~ModelFieldWidget() {};
			void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

			void localize() override;
			void refresh();

			inline Model::BaseModel * const		  getModel() { return _model; };
			inline const Model::BaseModel * const getModel() const { return _model; };
			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
			M * const getModel()
			{
				return static_cast<M *>( getModel() );
			}
			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
			const M * const getModel() const
			{
				return static_cast<M *>( getModel() );
			}

			void setModel( Model::BaseModel * const p_model );

		  signals:
			void onModelChanged( Model::BaseModel * const p_model );

		  protected:
			ModelFieldWidget( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			void _onModelDropped( Model::BaseModel * const p_model );

			QMimeData * _getDataForDrag() const override;

		  private:
			Model::BaseModel * _model = nullptr;

			QLabel * _label				  = nullptr;
			QLabel * _modelTypeIconWidget = nullptr;

			QString _placeholder = "Drag a model here";
		};
	} // namespace UI::Widget::CustomWidget
} // namespace VTX
#endif
