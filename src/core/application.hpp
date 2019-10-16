#ifndef __CORE_APPLICATION__
#define __CORE_APPLICATION__

namespace VTX
{
	namespace Core
	{
		template<typename T>
		class Application
		{
		public:
			static T& INSTANCE()
			{
				static T instance;
				return instance;
			}

			virtual void start() = 0;
			virtual void stop() = 0;

		protected:
			Application() = default;
			Application(const Application&) = delete;
			Application& operator=(const Application&) = delete;
			virtual ~Application() = default;
		};
	}
}

#endif