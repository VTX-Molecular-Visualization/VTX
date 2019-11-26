#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#include "../defines.hpp"
#include "../event/event.hpp"
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace View
	{
		class BaseModel;

		template<typename T>
		class BaseView
		{
		  public:
			BaseView() {}
			virtual ~BaseView() { _model = nullptr; };

			virtual void setModel( T * const p_model ) final { _model = p_model; }
			virtual void notify( Event::EVENT_MODEL ) {};

		  protected:
			T * _model = nullptr;
		};
	} // namespace View
} // namespace VTX
#endif
