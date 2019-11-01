#ifndef __BASE_COMPONENT__
#define __BASE_COMPONENT__

#include "../util/logger.hpp"
#include <vector>

namespace VTX
{
	namespace UI
	{
		class BaseComponent
		{
		  public:
			BaseComponent( bool * );
			~BaseComponent();
			virtual void display() final;
			virtual void init();
			bool		 isShown() { return *_show; }

		  protected:
			bool * _show;

			virtual void _addComponent( BaseComponent * ) final;
			virtual void _addComponents() {};
			virtual void _drawComponents() final;
			virtual void _draw() = 0;

		  private:
			bool						 _isInitialized = false;
			std::vector<BaseComponent *> _components
				= std::vector<BaseComponent *>();
		};
	} // namespace UI
} // namespace VTX
#endif
