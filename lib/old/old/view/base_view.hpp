#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#include "event/event.hpp"
#include "model/base_model.hpp"
#include <util/logger.hpp>

namespace VTX::MVC
{
	class MvcManager;
}

#define VTX_VIEW friend VTX::MVC::MvcManager;

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView
		{
			VTX_VIEW

		  public:
			virtual void notify( const Event::VTXEvent * const p_event )
			{
				if ( p_event->name == Event::Model::DATA_CHANGE )
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
	} // namespace View
} // namespace VTX
#endif
