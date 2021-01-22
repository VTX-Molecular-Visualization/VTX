#ifndef __VTX_UI_WIDGET_CUSTOM_TRANSFORM__
#define __VTX_UI_WIDGET_CUSTOM_TRANSFORM__

#ifdef _MSC_VER
#pragma once
#endif

#include "math/transform.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "vector3_widget.hpp"
#include <QBoxLayout>
#include <QFrame>
#include <QSpinBox>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				class TransformWidget : public BaseManualWidget<QFrame>
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

				  signals:
					void onValueChange( const Math::Transform & value ) const;

				  protected:
					TransformWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;
					void _refresh();

				  private:
					Vector3Widget * _positionWidget;
					Vector3Widget * _rotationWidget;
					Vector3Widget * _scaleWidget;

					void _onPositionChange( const Vec3f & p_position );
					void _onRotationChange( const Vec3f & p_euler );
					void _onScaleChange( const Vec3f & p_scale );

					Math::Transform _transform = Math::Transform();
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
