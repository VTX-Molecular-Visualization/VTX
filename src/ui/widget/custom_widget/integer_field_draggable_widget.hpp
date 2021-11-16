#ifndef __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD_DRAGGABLE__
#define __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD_DRAGGABLE__

#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class IntegerFieldDraggableWidget : public BaseManualWidget<QWidget>, public TMultiDataFieldEquatable<const int>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~IntegerFieldDraggableWidget() = default;

		const int getValue() const { return _value; };
		void	  setValue( const int p_value );

		void localize() override;

		void setMin( const int p_min );
		void setMax( const int p_max );
		void setMinMax( const int p_min, const int p_max );

		void setLabel( const QString & p_label );

		void setDragValueFactor( const float p_factor );
		void setEnabled( const bool p_enable );

		void separateChangeAndDrag( const bool p_separate );

		// MultiDataField Implementation //////////////////////////////
		void resetState() override;
		//////////////////////////////////////////////////////////////

	  signals:
		void onValueChange( const float p_value );
		void onValueDragged( const float p_delta );

	  protected:
		IntegerFieldDraggableWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		void paintEvent( QPaintEvent * event ) override;
		void mousePressEvent( QMouseEvent * p_event ) override;
		void mouseMoveEvent( QMouseEvent * p_event ) override;
		void mouseReleaseEvent( QMouseEvent * p_event ) override;

		void _onTextFieldEdited();
		void _onInternalValueChanged( const int p_newValue );
		void _onValueDragged( const int p_delta );

		// MultiDataField Implementation //////////////////////////////
		void		_displayDifferentsDataFeedback();
		const int & _getValue() const { return _value; }
		void		_setSingleValue( const int & p_value ) { setValue( p_value ); };
		//////////////////////////////////////////////////////////////

	  private:
		QLabel *	_label	   = nullptr;
		QLineEdit * _textField = nullptr;

		bool _separateChangeAndDrag = false;

		int _value = 0;
		int _min;
		int _max;

		float  _dragValueFactor = 1.0f;
		bool   _hasTarget		= false;
		QPoint _mousePressPos;

		bool _canDragAtPos( const QPoint & p_globalPos ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
