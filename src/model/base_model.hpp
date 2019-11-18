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
			BaseModel() = default;
			~BaseModel();
			virtual void init();

		  protected:
			template<typename T, typename = std::enable_if<std::is_base_of<View::BaseView<BaseModel>, T>::value>>
			void		 _addView( T * const );
			virtual void _addViews() {};
			virtual void _notifyViews( Event::EVENT_MODEL ) const final;

		  private:
			std::vector<View::BaseView<BaseModel> *> _views = std::vector<View::BaseView<BaseModel> *>();
		};
	} // namespace Model
} // namespace VTX
#endif
