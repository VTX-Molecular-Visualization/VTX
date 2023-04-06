#ifndef __VTX_UI_WIDGET_CUSTOM_VECTOR3__
#define __VTX_UI_WIDGET_CUSTOM_VECTOR3__

#include "float_field_draggable_widget.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QSpinBox>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class Vector3Widget : public BaseManualWidget<QWidget>, public TMultiDataField<const Vec3f>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		enum AxisMask : int
		{
			NONE = 0,

			X = 1 << 0,
			Y = 1 << 1,
			Z = 1 << 2,

			XY = X | Y,
			XZ = X | Z,
			YZ = Y | Z,

			XYZ = X | Y | Z,
		};

		~Vector3Widget() {};

		const Vec3f & getData() const;
		void		  setData( const Vec3f & p_vec3 );

		void localize() override;

		void setMin( const float p_min );
		void setMin( const Vec3f p_min );
		void setMax( const float p_max );
		void setMax( const Vec3f p_max );
		void setMinMax( const float p_min, const float p_max );
		void setMinMax( const Vec3f p_min, const Vec3f p_max );

		void setDragValueFactor( const float p_factor );

		// MultiDataField Implementation //////////////////////////////
		void resetState() override;
		void updateWithNewValue( const Vec3f & p_value ) override;
		//////////////////////////////////////////////////////////////

	  signals:
		void onValueChange( const Vec3f & p_value, const AxisMask & p_mask );
		void onValueDragged( const Vec3f & p_delta );

	  protected:
		Vector3Widget( QWidget * p_parent ) : BaseManualWidget( p_parent ), TMultiDataField<const Vec3f>() {};

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		// MultiDataField Implementation //////////////////////////////
		void _displayDifferentsDataFeedback() override;
		//////////////////////////////////////////////////////////////

		void _onInternalValueXChanged( const float p_newValue );
		void _onInternalValueYChanged( const float p_newValue );
		void _onInternalValueZChanged( const float p_newValue );

		void _onInternalValueXDragged( const float p_delta );
		void _onInternalValueYDragged( const float p_delta );
		void _onInternalValueZDragged( const float p_delta );

	  private:
		FloatFieldDraggableWidget * _widgetX = nullptr;
		FloatFieldDraggableWidget * _widgetY = nullptr;
		FloatFieldDraggableWidget * _widgetZ = nullptr;

		Vec3f _data;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
