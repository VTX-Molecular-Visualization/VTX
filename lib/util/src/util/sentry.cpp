#include "util/sentry.hpp"

namespace VTX::Util
{
	Sentry SentryTarget::newSentry() noexcept { return Sentry( _alive ); }
	SentryTarget::~SentryTarget()
	{
		if ( _alive )
			*_alive = false;
	}
	SentryTarget::SentryTarget( SentryTarget && p_other ) noexcept : _alive( p_other._alive )
	{
		p_other._alive = nullptr;
	}
	SentryTarget & SentryTarget::operator=( SentryTarget && p_other ) noexcept
	{
		if ( this == &p_other )
			return *this;

		if ( _alive )
			*_alive = false;

		_alive		   = p_other._alive;
		p_other._alive = nullptr;

		return *this;
	}
	Sentry::operator bool() const noexcept { return *_targetAlive; }
	Sentry::Sentry( std::shared_ptr<std::atomic_bool> p_ ) noexcept : _targetAlive( p_ ) {}
} // namespace VTX::Util
