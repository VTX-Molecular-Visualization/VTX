#ifndef __VTX_APP_CORE_SERIALIZATION_VERSION__
#define __VTX_APP_CORE_SERIALIZATION_VERSION__

namespace VTX::App::Core::Serialization
{
	class Version
	{
	  public:
		static const Version CURRENT;

	  public:
		Version();
		Version( const int p_major, const int p_minor, const int p_revision );

		friend bool operator==( const Version & p_lhs, const Version & p_rhs );
		friend bool operator!=( const Version & p_lhs, const Version & p_rhs );
		friend bool operator>( const Version & p_lhs, const Version & p_rhs );
		friend bool operator>=( const Version & p_lhs, const Version & p_rhs );
		friend bool operator<( const Version & p_lhs, const Version & p_rhs );
		friend bool operator<=( const Version & p_lhs, const Version & p_rhs );

		int major	 = 0;
		int minor	 = 0;
		int revision = 0;
	};

} // namespace VTX::App::Core::Serialization

#endif
