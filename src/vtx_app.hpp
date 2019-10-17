#ifndef __VTX_APP__
#define __VTX_APP__

namespace VTX
{
	class VTXApp final
	{
	public:
		static VTXApp& INSTANCE()
		{
			static VTXApp instance;
			return instance;
		}

		void start();
		void stop();

	private:
		VTXApp() = default;
		VTXApp(const VTXApp&) = delete;
		VTXApp& operator=(const VTXApp&) = delete;
		virtual ~VTXApp() = default;
	};
}

#endif