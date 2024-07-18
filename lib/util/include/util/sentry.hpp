#ifndef __VTX_UTIL_SENTRY__
#define __VTX_UTIL_SENTRY__

#include <memory>

namespace VTX::Util
{
	class Sentry;

	// Class that provide Sentries that check if their target still exists
	class SentryTarget
	{
	  public:
		SentryTarget() = default;
		~SentryTarget();
		SentryTarget( SentryTarget && ) noexcept;
		SentryTarget( const SentryTarget & ) noexcept = delete;
		SentryTarget & operator=( SentryTarget && ) noexcept;
		SentryTarget & operator=( const SentryTarget & ) noexcept = delete;

		Sentry newSentry() noexcept;

	  private:
		std::shared_ptr<bool> _alive = std::make_shared<bool>( true );
	};

	// Class responsible for saying if its target still exist or not
	class Sentry
	{
		friend SentryTarget;

	  public:
		Sentry() = delete;

		operator bool() noexcept;

	  private:
		Sentry( std::shared_ptr<bool> p_ ) noexcept;

		std::shared_ptr<bool> _targetAlive;
	};
} // namespace VTX::Util

#endif
