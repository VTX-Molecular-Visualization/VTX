#ifndef __VTX_UI_WIDGET_MODEL_DROP_AREA__
#define __VTX_UI_WIDGET_MODEL_DROP_AREA__

#include "id.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFrame>
#include <QString>
#include <QWidget>
#include <vector>
#include <type_traits>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
	}

	namespace UI::Widget::CustomWidget
	{
		class ModelDropArea : public BaseManualWidget<QFrame>
		{
			Q_OBJECT
			VTX_WIDGET

		  public:
			~ModelDropArea() {};
			void localize() override;

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

			void addTypeFilter( const VTX::ID::VTX_ID & p_modelID );

		  signals:
			void onModelChanged( Model::BaseModel * const p_model );

		  protected:
			ModelDropArea( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			void dragEnterEvent( QDragEnterEvent * event ) override;
			void dropEvent( QDropEvent * event ) override;

		  private:
			Model::BaseModel * _model = nullptr;

			std::vector<VTX::ID::VTX_ID> _filters = std::vector<VTX::ID::VTX_ID>();
		};
	} // namespace UI::Widget::CustomWidget
} // namespace VTX
#endif
