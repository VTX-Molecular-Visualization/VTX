#ifndef __VTX_UI_WIDGET_CUSTOM_TRANSFORM__
#define __VTX_UI_WIDGET_CUSTOM_TRANSFORM__

#include "math/transform.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "vector3_widget.hpp"
#include <QBoxLayout>
#include <QFrame>
#include <QSpinBox>

namespace VTX::UI::Widget::CustomWidget
{
	class TransformWidget : public BaseManualWidget<QFrame>, public TMultiDataField<const Math::Transform>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~TransformWidget() {};

		void setData( const Math::Transform & p_data )
		{
			_transform = p_data;
			_refresh();
		};

		void localize() override;

		// MultiDataField Implementation //////////////////////////////
		void resetState() override;
		void updateWithNewValue( const Math::Transform & p_value ) override;
		//////////////////////////////////////////////////////////////

		void displayPosition( const bool p_display ) const;
		void displayRotation( const bool p_display ) const;
		void displayScale( const bool p_display ) const;

	  signals:
		void onValueChange( const Math::Transform & p_value ) const;
		void onPositionDragged( const Vec3f & p_delta ) const;
		void onRotationDragged( const Vec3f & p_delta ) const;
		void onScaleDragged( const Vec3f & p_delta ) const;

	  protected:
		TransformWidget( QWidget * p_parent ) :
			BaseManualWidget( p_parent ), TMultiDataField<const Math::Transform>() {};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		// MultiDataField Implementation //////////////////////////////
		void _displayDifferentsDataFeedback();
		//////////////////////////////////////////////////////////////

	  private:
		Vector3Widget * _positionWidget = nullptr;
		Vector3Widget * _rotationWidget = nullptr;
		Vector3Widget * _scaleWidget	= nullptr;

		QLabel * _positionLabel = nullptr;
		QLabel * _rotationLabel = nullptr;
		QLabel * _scaleLabel	= nullptr;

		void _onPositionChange( const Vec3f & p_position );
		void _onRotationChange( const Vec3f & p_euler );
		void _onScaleChange( const Vec3f & p_scale );

		void _onPositionDragged( const Vec3f & p_delta );
		void _onRotationDragged( const Vec3f & p_delta );
		void _onScaleDragged( const Vec3f & p_delta );

		Math::Transform _transform = Math::Transform();
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
