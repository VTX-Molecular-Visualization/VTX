#ifndef __VTX_BASE_UPDATABLE__
#define __VTX_BASE_UPDATABLE__

namespace VTX
{
	namespace Generic
	{
		class BaseUpdatable
		{
		  public:
			virtual ~BaseUpdatable() = default;

			virtual void update( const float & p_deltaTime ) = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
