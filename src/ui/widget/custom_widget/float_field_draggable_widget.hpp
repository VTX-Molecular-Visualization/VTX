#ifndef __VTX_UI_WIDGET_CUSTOM_FLOAT_FIELD_DRAGGABLE__
#define __VTX_UI_WIDGET_CUSTOM_FLOAT_FIELD__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class FloatFieldDraggableWidget : public BaseManualWidget<QWidget>, public TMultiDataFieldEquatable<float>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~FloatFieldDraggableWidget() = default;

		const float getValue() const { return _value; };
		void		setValue( const float p_value );

		void localize() override;
		void setNbDecimals( const int p_nbDecimals );

		void setMin( const float p_min );
		void setMax( const float p_max );
		void setMinMax( const float p_min, const float p_max );

		void setLabel( const QString & p_label );

		void setDragValueFactor( const float p_factor );
		void setEnabled( const bool p_enable );

		// MultiDataField Implementation //////////////////////////////
		void resetState() override;
		//////////////////////////////////////////////////////////////

	  signals:
		void onValueChange( const float p_value );

	  protected:
		FloatFieldDraggableWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		void paintEvent( QPaintEvent * event ) override;
		void mousePressEvent( QMouseEvent * p_event ) override;
		void mouseMoveEvent( QMouseEvent * p_event ) override;
		void mouseReleaseEvent( QMouseEvent * p_event ) override;

		void _onTextFieldEdited();
		void _onInternalValueChanged( const float p_newValue );

		// MultiDataField Implementation //////////////////////////////
		void		  _displayDifferentsDataFeedback();
		const float & _getValue() const { return _value; }
		void		  _setSingleValue( const float & p_value ) { setValue( p_value ); };
		//////////////////////////////////////////////////////////////

	  private:
		QLabel *	_label	   = nullptr;
		QLineEdit * _textField = nullptr;

		int	  _nbDecimals = 2;
		float _epsilon	  = 0.01f;

		float _value = 0;
		float _min;
		float _max;

		float  _dragValueFactor = 1.0f;
		bool   _hasTarget		= false;
		QPoint _mousePressPos;

		bool _canDragAtPos( const QPoint & p_globalPos ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
