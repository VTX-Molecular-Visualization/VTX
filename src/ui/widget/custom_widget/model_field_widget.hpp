#ifndef __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__
#define __VTX_UI_WIDGET_MODEL_FIELD_WIDGET__

#include "ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QDragEnterEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
	}

	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				class ModelFieldWidget : public BaseManualWidget<QFrame>
				{
					Q_OBJECT
					VTX_WIDGET

				  public:
					~ModelFieldWidget() {};
					void localize() override;

					inline Model::BaseModel * const		  getModel() { return _model; };
					inline const Model::BaseModel * const getModel() const { return _model; };

				  signals:
					void dataChanged();

				  protected:
					ModelFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

					void dragEnterEvent( QDragEnterEvent * event ) override;
					void dropEvent( QDropEvent * event ) override;

				  private:
					Model::BaseModel * _model = nullptr;
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
