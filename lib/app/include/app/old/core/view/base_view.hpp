#ifndef __VTX_VIEW_BASE_VIEW__
#define __VTX_VIEW_BASE_VIEW__

#include "app/old/core/event/vtx_event.hpp"
#include "app/old/core/model/base_model.hpp"
#include <util/logger.hpp>

namespace VTX::App::Old::Manager
{
	class MvcManager;
}

#define VTX_VIEW friend VTX::App::Old::Manager::MvcManager;

namespace VTX::App::Old::Core::View
{
	template<typename T, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, T>::value>>
	class BaseView
	{
		VTX_VIEW

	  public:
		virtual void notify( const App::Old::Core::Event::VTXEvent * const p_event )
		{
			if ( p_event->name == App::Old::Event::Model::DATA_CHANGE )
			{
				_refreshView();
			}
		}

	  protected:
		T * const _model;

		explicit BaseView( T * const p_model ) : _model( p_model ) {}
		virtual ~BaseView() = default;
		virtual void _refreshView() {}
	};
} // namespace VTX::App::Old::Core::View
#endif
