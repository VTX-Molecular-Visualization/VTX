#ifndef __VTX_UI_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include <QWidget>
#include <type_traits>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				template<typename W, typename = std::enable_if<std::is_base_of<W, QWidget>::value>>
				class DockWindowMainWidget : public W
				{
				  public:
					DockWindowMainWidget( QWidget * p_parent = nullptr ) : W( p_parent ) {};

					QSize sizeHint() const override
					{
						if ( _sizeHintOverrided )
							return _sizeHint;
						else
							return W::sizeHint();
					};

					inline void setSizeHint( const QSize & p_sizeHint )
					{
						_sizeHint		   = p_sizeHint;
						_sizeHintOverrided = true;
					};

				  private:
					bool  _sizeHintOverrided = false;
					QSize _sizeHint;
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
