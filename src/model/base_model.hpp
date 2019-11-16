#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

#include "../view/base_view.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class BaseModel
		{
		  public:
		  private:
			std::vector<View::BaseView<BaseModel> *> _components = std::vector<View::BaseView<BaseModel> *>();
		};
	} // namespace Model
} // namespace VTX
#endif
