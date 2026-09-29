#include "util/sentry.hpp"
#include <utility>

namespace VTX::Util
{
	Sentry SentryTarget::newSentry() noexcept { return Sentry( _alive ); }

	SentryTarget::~SentryTarget()
	{
		if ( _alive )
		{
			*_alive = false;
		}
	}

	SentryTarget::SentryTarget( SentryTarget && p_other ) noexcept : _alive( std::move( p_other._alive ) ) {}

	SentryTarget & SentryTarget::operator=( SentryTarget && p_other ) noexcept
	{
		if ( this == &p_other )
		{
			return *this;
		}

		if ( _alive )
		{
			*_alive = false;
		}

		_alive = std::move( p_other._alive );

		return *this;
	}

	Sentry::operator bool() const noexcept { return *_targetAlive; }

	Sentry::Sentry( std::shared_ptr<std::atomic_bool> p_ ) noexcept : _targetAlive( std::move( p_ ) ) {}
} // namespace VTX::Util
