#ifndef __VTX_UTIL_SENTRY__
#define __VTX_UTIL_SENTRY__

#include <memory>
#include <thread>

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

		// Spawn a new sentry that watch this object's instance
		Sentry newSentry() noexcept;

	  private:
		std::shared_ptr<std::atomic_bool> _alive = std::make_shared<std::atomic_bool>( true );
	};

	// Class responsible for saying if its target still exist or not
	class Sentry
	{
		friend SentryTarget;

	  public:
		Sentry() = delete;

		// Returns wether the target still exists
		operator bool() const noexcept;

	  private:
		Sentry( std::shared_ptr<std::atomic_bool> p_ ) noexcept;

		std::shared_ptr<std::atomic_bool> _targetAlive;
	};
} // namespace VTX::Util

#endif
