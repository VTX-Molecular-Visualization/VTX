#ifndef __VTX_APP__
#define __VTX_APP__

#include "core/application.hpp"

namespace VTX
{
	class VTXApp final : public Core::Application<VTXApp>
	{
	public:
		virtual void start() override;
		virtual void stop() override;
	};
}

#endif