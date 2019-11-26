#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#include "../model/base_model.hpp"
#include "base_view.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView3D : public BaseView<T>
		{
		  public:
			virtual void render( const uint p_time ) = 0;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
