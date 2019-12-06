#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace State
	{
		enum class STATE_NAME
		{
			LOADING,
			VISUALIZATION
		};

		class BaseState
		{
		  public:
			BaseState() = default;

			virtual STATE_NAME getName() const = 0;
			virtual void	   enter()		   = 0;
			virtual void	   exit()		   = 0;
			virtual void	   update() {}
		};
	} // namespace State
} // namespace VTX
#endif
