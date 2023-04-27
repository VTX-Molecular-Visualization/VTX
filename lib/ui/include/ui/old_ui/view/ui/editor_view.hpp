#ifndef __VTX_VIEW_UI_EDITOR_TARGET_VIEW__
#define __VTX_VIEW_UI_EDITOR_TARGET_VIEW__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/view/ui/editor_view.hpp"
#include <app/core/event/vtx_event.hpp>
#include <app/core/view/base_view.hpp>
#include <app/core/view/callback_view.hpp>
#include <app/mvc.hpp>

namespace VTX::View::UI
{
	template<typename T>
	class EditorView
	{
	  public:
		EditorView( const ID::VTX_ID & p_viewId ) : _viewId( p_viewId ) {}
		virtual ~EditorView()
		{
			if ( _view != nullptr && VTX::MVC_MANAGER().hasView( _target, _viewId ) )
				VTX::MVC_MANAGER().deleteView( _target, _viewId );
		}

		void createTempView( T * const p_target )
		{
			if ( _view != nullptr && VTX::MVC_MANAGER().hasView( _target, _viewId ) )
				VTX::MVC_MANAGER().deleteView( _target, _viewId );

			_target = p_target;

			_view
				= VTX::MVC_MANAGER().instantiateView<App::Core::View::CallbackView<T, EditorView>>( p_target, _viewId );
			_view->setCallback( this, &EditorView::_catchModelEvent );
		}

	  protected:
		virtual void _catchModelEvent( const VTX::App::Core::Event::VTXEvent * const p_event ) = 0;

	  private:
		const ID::VTX_ID							   _viewId;
		App::Core::View::CallbackView<T, EditorView> * _view   = nullptr;
		const T *									   _target = nullptr;
	};

} // namespace VTX::View::UI

#endif
