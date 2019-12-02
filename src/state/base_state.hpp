#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

namespace VTX
{
	namespace State
	{
		enum class STATE_NAME
		{
			LOADING,
			VISU
		};

		class BaseState
		{
		  public:
			BaseState() = default;

			virtual STATE_NAME getName() const = 0;
			virtual void	   enter()		   = 0;
			virtual void	   exit()		   = 0;
		};
	} // namespace State
} // namespace VTX
#endif
