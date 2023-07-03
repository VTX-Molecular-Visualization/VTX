#ifndef __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__
#define __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__

#include "ui/old_ui/ui/draggable_item.hpp"
#include "ui/old_ui/ui/widget/custom_widget/model_drop_area.hpp"
#include <QLabel>
#include <QWidget>
#include <app/old/id.hpp>
#include <type_traits>

namespace VTX
{
	namespace App::Old::Core::Model
	{
		class BaseModel;
	} // namespace App::Old::Core::Model

	namespace UI::Widget::CustomWidget
	{
		class ModelFieldWidget : public CustomWidget::ModelDropArea, public DraggableItem
		{
			Q_OBJECT
			VTX_WIDGET

		  public:
			~ModelFieldWidget() {};
			void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;

			void localize() override;
			void refresh();

			inline App::Old::Core::Model::BaseModel * const		 getModel() { return _model; };
			inline const App::Old::Core::Model::BaseModel * const getModel() const { return _model; };
			template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
			M * const getModel()
			{
				return static_cast<M *>( getModel() );
			}
			template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
			const M * const getModel() const
			{
				return static_cast<M *>( getModel() );
			}

			void setModel( App::Old::Core::Model::BaseModel * const p_model );

		  signals:
			void onModelChanged( App::Old::Core::Model::BaseModel * const p_model );

		  protected:
			ModelFieldWidget( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			void _onModelDropped( App::Old::Core::Model::BaseModel * const p_model );

			QMimeData * _getDataForDrag() const override;

		  private:
			App::Old::Core::Model::BaseModel * _model = nullptr;

			QLabel * _label				  = nullptr;
			QLabel * _modelTypeIconWidget = nullptr;

			QString _placeholder = "Drag a model here";
		};
	} // namespace UI::Widget::CustomWidget
} // namespace VTX
#endif
