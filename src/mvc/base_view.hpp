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

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView
		{
		  public:
			void		 setModel( const T & p_model ) { _model = p_model; }
			virtual void notify( Event::EVENT_MODEL ) {};

		  protected:
			T _model;
		};
	} // namespace View
} // namespace VTX
#endif
