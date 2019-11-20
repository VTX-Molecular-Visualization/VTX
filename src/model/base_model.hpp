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
			virtual ~BaseModel();
			virtual void init();
			virtual void update();

		  protected:
			void		 _addView( const std::shared_ptr<View::BaseView<BaseModel>> );
			virtual void _addViews() {};
			virtual void _notifyViews( const Event::EVENT_MODEL ) const final;

		  private:
			std::vector<std::shared_ptr<View::BaseView<BaseModel>>> _views
				= std::vector<std::shared_ptr<View::BaseView<BaseModel>>>();
		};
	} // namespace Model
} // namespace VTX
#endif
