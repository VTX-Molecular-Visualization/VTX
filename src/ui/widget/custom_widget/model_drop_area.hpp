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

			void addTypeFilter( const VTX::ID::VTX_ID & p_modelID );

		  signals:
			void onModelDropped( Model::BaseModel * const p_model );

		  protected:
			ModelDropArea( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			void dragEnterEvent( QDragEnterEvent * event ) override;
			void dropEvent( QDropEvent * event ) override;

		  private:
			std::vector<VTX::ID::VTX_ID> _filters = std::vector<VTX::ID::VTX_ID>();
		};
	} // namespace UI::Widget::CustomWidget
} // namespace VTX
#endif
