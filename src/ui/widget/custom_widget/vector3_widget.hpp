#ifndef __VTX_UI_WIDGET_CUSTOM_VECTOR3__
#define __VTX_UI_WIDGET_CUSTOM_VECTOR3__

#ifdef _MSC_VER
#pragma once
#endif

#include "float_field_draggable_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QSpinBox>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class Vector3Widget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		Q_OBJECT

	  private:
	  public:
		~Vector3Widget() {};

		const Vec3f & getData() const;
		void		  setData( const Vec3f & p_vec3 );

		void localize() override;

		void setMin( const float p_min );
		void setMax( const float p_max );
		void setMinMax( const float p_min, const float p_max );

		void setDragValueFactor( const float p_factor );

	  signals:
		void onValueChange( const Vec3f & value );

	  protected:
		Vector3Widget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		void _onInternalValueXChanged( const float p_newValue );
		void _onInternalValueYChanged( const float p_newValue );
		void _onInternalValueZChanged( const float p_newValue );

	  private:
		FloatFieldDraggableWidget * _widgetX = nullptr;
		FloatFieldDraggableWidget * _widgetY = nullptr;
		FloatFieldDraggableWidget * _widgetZ = nullptr;

		Vec3f _data;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
