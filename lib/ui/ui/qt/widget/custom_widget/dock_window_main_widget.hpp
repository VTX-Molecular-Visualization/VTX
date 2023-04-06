#ifndef __VTX_UI_QT_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__
#define __VTX_UI_QT_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__

#include <QFocusEvent>
#include <QWidget>
#include <type_traits>

namespace VTX::UI::QT::Widget::CustomWidget
{
	template<typename W, typename = std::enable_if<std::is_base_of<W, QWidget>::value>>
	class DockWindowMainWidget : public W
	{
	  public:
		DockWindowMainWidget( const QSize & p_sizeHint,
							  const QSize & p_minimumSizeHint,
							  QWidget *		p_parent = nullptr ) :
			_sizeHint( p_sizeHint ),
			_sizeHintOverrided( true ), _minimumSizeHint( p_minimumSizeHint ), _minimumSizeHintOverrided( true ),
			W( p_parent )
		{
			W::setMinimumSize( _minimumSizeHint );
			W::setFocusPolicy( Qt::FocusPolicy( Qt::FocusPolicy::ClickFocus | Qt::FocusPolicy::TabFocus ) );
		};
		DockWindowMainWidget( QWidget * p_parent = nullptr ) : W( p_parent ) {};

		QSize sizeHint() const override
		{
			if ( _sizeHintOverrided )
				return _sizeHint;
			else
				return W::sizeHint();
		};

		QSize minimumSizeHint() const override
		{
			if ( _minimumSizeHintOverrided )
				return _minimumSizeHint;
			else
				return W::minimumSizeHint();
		};

		inline void setSizeHint( const QSize & p_sizeHint )
		{
			_sizeHint		   = p_sizeHint;
			_sizeHintOverrided = true;
		};
		inline void setMinimumSizeHint( const QSize & p_sizeHint )
		{
			_minimumSizeHint		  = p_sizeHint;
			_minimumSizeHintOverrided = true;
			W::setMinimumSize( _minimumSizeHint );
		};

	  private:
		bool  _sizeHintOverrided = false;
		QSize _sizeHint;
		bool  _minimumSizeHintOverrided = false;
		QSize _minimumSizeHint;
	};

} // namespace VTX::UI::QT::Widget::CustomWidget
#endif
