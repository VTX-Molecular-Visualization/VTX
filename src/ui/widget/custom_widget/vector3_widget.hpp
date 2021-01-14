#ifndef __VTX_UI_WIDGET_CUSTOM_VECTOR3__
#define __VTX_UI_WIDGET_CUSTOM_VECTOR3__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QSpinBox>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				class Vector3Widget : public BaseManualWidget<QWidget>
				{
					VTX_WIDGET
					Q_OBJECT

				  public:
					~Vector3Widget() {};

					const Vec3f & getData() const { return _data; };
					void		  setData( const Vec3f & p_vec3 )
					{
						_data = p_vec3;
						_refresh();
					};

					void localize() override;

				  signals:
					void onValueChange( const Vec3f & value );

				  protected:
					Vector3Widget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;
					void _refresh();
					void _onInternalValueXChanged( const double p_newValue );
					void _onInternalValueYChanged( const double p_newValue );
					void _onInternalValueZChanged( const double p_newValue );

				  private:
					QDoubleSpinBox * _widgetX = nullptr;
					QDoubleSpinBox * _widgetY = nullptr;
					QDoubleSpinBox * _widgetZ = nullptr;

					Vec3f _data;
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
