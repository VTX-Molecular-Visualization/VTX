#ifndef __VTX_UI_WIDGET_CUSTOM_TRANSFORM__
#define __VTX_UI_WIDGET_CUSTOM_TRANSFORM__

#include "app/internal/math/transform.hpp"
#include "ui/old_ui/ui/multi_data_field.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "vector3_widget.hpp"
#include <QBoxLayout>
#include <QFrame>
#include <QSpinBox>
#include <app/component/generic/base_transformable.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	class TransformWidget :
		public BaseManualWidget<QFrame>,
		public TMultiDataField<const App::Internal::Math::Transform>
	{
		VTX_WIDGET
		Q_OBJECT

	  protected:
		enum class Field
		{
			Position,
			Euler,
			Scale
		};

	  public:
		~TransformWidget() {};

		void setData( const App::Internal::Math::Transform & p_data )
		{
			_transform = p_data;
			_refresh();
		};

		void localize() override;

		// MultiDataField Implementation //////////////////////////////
		void resetState() override;
		void updateWithNewValue( const App::Internal::Math::Transform & p_value ) override;
		//////////////////////////////////////////////////////////////

		void displayPosition( const bool p_display ) const;
		void displayRotation( const bool p_display ) const;
		void displayScale( const bool p_display ) const;

	  signals:
		void onValueChange( const App::Internal::Math::Transform & p_value,
							const App::Internal::Math::TRANSFORM_COMPOSANT_MASK & ) const;
		void onPositionDragged( const Vec3f & p_delta ) const;
		void onRotationDragged( const Vec3f & p_delta ) const;
		void onScaleDragged( const Vec3f & p_delta ) const;

	  protected:
		TransformWidget( QWidget * p_parent ) :
			BaseManualWidget( p_parent ), TMultiDataField<const App::Internal::Math::Transform>() {};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		// MultiDataField Implementation //////////////////////////////
		void _displayDifferentsDataFeedback() override;
		//////////////////////////////////////////////////////////////

	  private:
		Vector3Widget * _positionWidget = nullptr;
		Vector3Widget * _rotationWidget = nullptr;
		Vector3Widget * _scaleWidget	= nullptr;

		QLabel * _positionLabel = nullptr;
		QLabel * _rotationLabel = nullptr;
		QLabel * _scaleLabel	= nullptr;

		void _onPositionChange( const Vec3f & p_position, const Vector3Widget::AxisMask & p_axisMask );
		void _onRotationChange( const Vec3f & p_euler, const Vector3Widget::AxisMask & p_axisMask );
		void _onScaleChange( const Vec3f & p_scale, const Vector3Widget::AxisMask & p_axisMask );

		void _onPositionDragged( const Vec3f & p_delta );
		void _onRotationDragged( const Vec3f & p_delta );
		void _onScaleDragged( const Vec3f & p_delta );

		Vec3f _applyVectorWithMask( const Vec3f &					p_base,
									const Vec3f &					p_vector,
									const Vector3Widget::AxisMask & p_mask );

		App::Internal::Math::TRANSFORM_COMPOSANT_MASK _generateTransformMask(
			const Field &					p_field,
			const Vector3Widget::AxisMask & p_axis );

		App::Internal::Math::Transform _transform = App::Internal::Math::Transform();
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
