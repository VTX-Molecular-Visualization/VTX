#include <memory>
#include <qapplication.h>

namespace VTX::test
{
	struct setup_env
	{
		int								  i = 0;
		std::unique_ptr<QCoreApplication> papp { std::make_unique<QCoreApplication>( i, nullptr ) };
	};
} // namespace VTX::test
