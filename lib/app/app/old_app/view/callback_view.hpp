#ifndef __VTX_VIEW_CALLBACK_VIEW__
#define __VTX_VIEW_CALLBACK_VIEW__

#include "base_view.hpp"
#include "old_app/event/event.hpp"

namespace VTX::View
{
	template<typename T1, typename T2>
	class CallbackView : public View::BaseView<T1>
	{
		VTX_VIEW

	  public:
		void setCallback( T2 * const p_caller, void ( T2::*p_notifyCallback )( const Event::VTXEvent * const ) )
		{
			_notifyTarget	= p_caller;
			_notifyCallback = p_notifyCallback;
		}
		void setCallback( T2 * const p_caller,
						  void ( T2::*p_notifyCallback )( const T1 * const p_model, const Event::VTXEvent * const ) )
		{
			_notifyTarget			 = p_caller;
			_notifyWithModelCallback = p_notifyCallback;
		}

		void notify( const Event::VTXEvent * const p_event ) override
		{
			if ( _notifyCallback != nullptr )
				( _notifyTarget->*_notifyCallback )( p_event );

			if ( _notifyWithModelCallback != nullptr )
				( _notifyTarget->*_notifyWithModelCallback )( this->_model, p_event );
		}

	  protected:
		CallbackView( T1 * const p_model ) : View::BaseView<T1>( p_model ) {};

	  private:
		T2 * _notifyTarget																				  = nullptr;
		void ( T2::*_notifyCallback )( const Event::VTXEvent * const )									  = nullptr;
		void ( T2::*_notifyWithModelCallback )( const T1 * const p_model, const Event::VTXEvent * const ) = nullptr;
	};

} // namespace VTX::View

#endif
