#ifndef __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD_DRAGGABLE__
#define __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD_DRAGGABLE__

#include "ui/old_ui/ui/multi_data_field.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>
#include <QString>
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

		QLabel * getLabelWidget() { return _label; }
		void	 setLabel( const QString & p_label );

		void setDragValueFactor( const float p_factor );
		void setEnabled( const bool p_enable );

		void separateChangeAndDrag( const bool p_separate );

		// MultiDataField Implementation //////////////////////////////
		void resetState() override;
		//////////////////////////////////////////////////////////////

	  signals:
		void onValueChange( const int p_value );
		void onValueDragged( const int p_delta );

	  protected:
		IntegerFieldDraggableWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		bool eventFilter( QObject * p_obj, QEvent * p_event ) override;

		void paintEvent( QPaintEvent * event ) override;
		void labelMousePressEvent( QMouseEvent * p_event );
		void labelMouseMoveEvent( QMouseEvent * p_event );
		void labelMouseReleaseEvent( QMouseEvent * p_event );

		void _onTextFieldEdited();
		void _onInternalValueChanged( const int p_newValue );
		void _onValueDragged( const int p_delta );

		QString getDisplayedText( const int p_value ) const;

		// MultiDataField Implementation //////////////////////////////
		void		_displayDifferentsDataFeedback() override;
		const int & _getValue() const override { return _value; }
		void		_setSingleValue( const int & p_value ) override { setValue( p_value ); };
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
