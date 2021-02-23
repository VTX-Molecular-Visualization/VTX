#ifndef __VTX_UI_WIDGET_CUSTOM_VECTOR3__
#define __VTX_UI_WIDGET_CUSTOM_VECTOR3__

#ifdef _MSC_VER
#pragma once
#endif

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
		enum class DragTarget
		{
			NONE,
			X,
			Y,
			Z,
		};

	  public:
		~Vector3Widget() {};

		const Vec3f & getData() const { return _data; };
		void		  setData( const Vec3f & p_vec3 )
		{
			_data = p_vec3;
			_refresh();
		};

		void localize() override;

		void setSpinBoxMin( const double p_min )
		{
			_widgetX->setMinimum( p_min );
			_widgetY->setMinimum( p_min );
			_widgetZ->setMinimum( p_min );
		};
		void setSpinBoxMax( const double p_max )
		{
			_widgetX->setMaximum( p_max );
			_widgetY->setMaximum( p_max );
			_widgetZ->setMaximum( p_max );
		};
		void setSpinBoxMinMax( const double p_min, const double p_max )
		{
			setSpinBoxMin( p_min );
			setSpinBoxMax( p_max );
		}
		void setSpinBoxStep( const double p_step )
		{
			_widgetX->setSingleStep( p_step );
			_widgetY->setSingleStep( p_step );
			_widgetZ->setSingleStep( p_step );
		};
		void setDragValueFactor( const float p_factor ) { _dragValueFactor = p_factor; }

	  signals:
		void onValueChange( const Vec3f & value );

	  protected:
		Vector3Widget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		void paintEvent( QPaintEvent * event ) override;

		void mousePressEvent( QMouseEvent * p_event ) override;
		void mouseMoveEvent( QMouseEvent * p_event ) override;
		void mouseReleaseEvent( QMouseEvent * p_event ) override;

		void _onInternalValueXChanged( const float p_newValue );
		void _onInternalValueYChanged( const float p_newValue );
		void _onInternalValueZChanged( const float p_newValue );

	  private:
		QLabel * _widgetLabelX = nullptr;
		QLabel * _widgetLabelY = nullptr;
		QLabel * _widgetLabelZ = nullptr;

		QDoubleSpinBox * _widgetX = nullptr;
		QDoubleSpinBox * _widgetY = nullptr;
		QDoubleSpinBox * _widgetZ = nullptr;

		Vec3f	   _data;
		float	   _dragValueFactor = 1.0f;
		DragTarget _dragTarget		= DragTarget::NONE;
		QPoint	   _mousePressPos;

		DragTarget _getDragTarget( const QPoint & p_globalPos ) const;
		bool	   _isOnWidget( const QPoint & p_globalPos, const QWidget & p_widget ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
