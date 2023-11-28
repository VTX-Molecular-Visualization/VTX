#include "app/core/serialization/version.hpp"
#include "app/info.hpp"

namespace VTX::App::Core::Serialization
{
	const Version Version::CURRENT = Version( Info::VERSION_MAJOR, Info::VERSION_MINOR, Info::VERSION_REVISION );

	Version::Version() = default;
	Version::Version( const int p_major, const int p_minor, const int p_revision ) :
		major( p_major ), minor( p_minor ), revision( p_revision )
	{
	}

	bool operator==( const Version & p_lhs, const Version & p_rhs )
	{
		return p_lhs.major == p_rhs.major && p_lhs.minor == p_rhs.minor && p_lhs.revision == p_rhs.revision;
	}
	bool operator!=( const Version & p_lhs, const Version & p_rhs )
	{
		return p_lhs.major != p_rhs.major || p_lhs.minor != p_rhs.minor || p_lhs.revision != p_rhs.revision;
	}
	bool operator>( const Version & p_lhs, const Version & p_rhs )
	{
		return p_lhs.major > p_rhs.major || p_lhs.minor > p_rhs.minor || p_lhs.revision > p_rhs.revision;
	}
	bool operator>=( const Version & p_lhs, const Version & p_rhs )
	{
		return p_lhs.major >= p_rhs.major || p_lhs.minor >= p_rhs.minor || p_lhs.revision >= p_rhs.revision;
	}
	bool operator<( const Version & p_lhs, const Version & p_rhs )
	{
		return p_lhs.major < p_rhs.major || p_lhs.minor < p_rhs.minor || p_lhs.revision < p_rhs.revision;
	}
	bool operator<=( const Version & p_lhs, const Version & p_rhs )
	{
		return p_lhs.major <= p_rhs.major || p_lhs.minor <= p_rhs.minor || p_lhs.revision <= p_rhs.revision;
	}

} // namespace VTX::App::Core::Serialization
