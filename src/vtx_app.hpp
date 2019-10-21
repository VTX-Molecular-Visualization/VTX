#ifndef __VTX_APP__
#define __VTX_APP__

#include "util/logger.hpp"

namespace VTX
{
	class VTXApp final
	{
	  public:
		static VTXApp & INSTANCE()
		{
			static VTXApp instance;
			return instance;
		}

		void init();
		void start();
		void stop();

	  private:
		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp()							 = default;
	};
} // namespace VTX

#endif
