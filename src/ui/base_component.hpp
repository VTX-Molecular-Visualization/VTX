#ifndef __BASE_COMPONENT__
#define __BASE_COMPONENT__

#include <vector>

namespace VTX
{
	namespace UI
	{
		class BaseComponent
		{
		  public:
			BaseComponent();
			~BaseComponent();
			virtual void display() final;

		  protected:
			virtual void _draw() = 0;
			virtual void _addComponent( BaseComponent * ) final;
			virtual void _addComponents() {};

		  private:
			std::vector<BaseComponent *> _components
				= std::vector<BaseComponent *>();
		};
	} // namespace UI
} // namespace VTX
#endif
