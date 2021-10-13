#ifndef __VTX_UI_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__

#include "controller/shortcut.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include <QFocusEvent>
#include <QWidget>
#include <type_traits>

namespace VTX::UI::Widget::CustomWidget
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
			setMinimumSize( _minimumSizeHint );
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
			setMinimumSize( _minimumSizeHint );
		};

		void setFocusGroup( const Controller::SHORTCUTGROUP & p_focusGroup )
		{
			setFocusPolicy( Qt::FocusPolicy( Qt::FocusPolicy::ClickFocus | Qt::FocusPolicy::TabFocus ) );
			_focusGroup = p_focusGroup;
		}
		void focusInEvent( QFocusEvent * p_event ) override
		{
			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->getItem<Controller::Shortcut>( ID::Controller::SHORTCUT )
				->setGroup( _focusGroup );
		}
		void focusOutEvent( QFocusEvent * p_event ) override
		{
			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->getItem<Controller::Shortcut>( ID::Controller::SHORTCUT )
				->removeGroup( _focusGroup );
		}

	  private:
		bool  _sizeHintOverrided = false;
		QSize _sizeHint;
		bool  _minimumSizeHintOverrided = false;
		QSize _minimumSizeHint;

		Controller::SHORTCUTGROUP _focusGroup = Controller::SHORTCUTGROUP::DEFAULT;
	};

} // namespace VTX::UI::Widget::CustomWidget
#endif
