#include <qapplication.h>

namespace VTX::test
{
	struct setup_env
	{
		int				 i = 0;
		QCoreApplication app { i, nullptr };
	};
} // namespace VTX::test
