#ifndef __VTX_VIEW_UI_EDITOR_TARGET_VIEW__
#define __VTX_VIEW_UI_EDITOR_TARGET_VIEW__

#include "old_ui/ui/widget/base_manual_widget.hpp"
#include "old_ui/view/ui/editor_view.hpp"
#include <app/old_app/event/event.hpp>
#include <app/old_app/mvc/mvc_manager.hpp>
#include <app/old_app/view/base_view.hpp>
#include <app/old_app/view/callback_view.hpp>

namespace VTX::View::UI
{
	template<typename T>
	class EditorView
	{
	  public:
		EditorView( const ID::VTX_ID & p_viewId ) : _viewId( p_viewId ) {}
		virtual ~EditorView()
		{
			if ( _view != nullptr && MVC::MvcManager::get().hasView( _target, _viewId ) )
				MVC::MvcManager::get().deleteView( _target, _viewId );
		}

		void createTempView( T * const p_target )
		{
			if ( _view != nullptr && MVC::MvcManager::get().hasView( _target, _viewId ) )
				MVC::MvcManager::get().deleteView( _target, _viewId );

			_target = p_target;

			_view = MVC::MvcManager::get().instantiateView<CallbackView<T, EditorView>>( p_target, _viewId );
			_view->setCallback( this, &EditorView::_catchModelEvent );
		}

	  protected:
		virtual void _catchModelEvent( const VTX::Event::VTXEvent * const p_event ) = 0;

	  private:
		const ID::VTX_ID			  _viewId;
		CallbackView<T, EditorView> * _view	  = nullptr;
		const T *					  _target = nullptr;
	};

} // namespace VTX::View::UI

#endif
