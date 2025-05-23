#ifndef __VTX_UTIL_URL__
#define __VTX_UTIL_URL__

#include <string>

namespace VTX::Util::Url
{
	const char * systemReplacementToken();
	const char * rcsbPdbDownloadUrlTemplate();
	const char * rcsbPdbDownloadBaseUrl();
	const char * rcsbPdbDownloadFileExtension();

	/**
	 * @brief Class representing the ID of a system
	 */
	class SystemId
	{
	  public:
		/*explicit*/ inline SystemId( const char * p_ ) : str( p_ ) {}

		std::string str;
		inline		operator std::string() noexcept { return str; }
		inline		operator const std::string() const noexcept { return str; }
	};

	/**
	 * @brief String template serving as a base to create a fully functionnal URL.
	 */
	class UrlTemplate
	{
	  public:
		explicit inline UrlTemplate( const char * p_ ) : str( p_ ) {}

		std::string str;
		inline		operator std::string() noexcept { return str; }
		inline		operator const std::string() const noexcept { return str; }

		bool hasReplacementToken() const noexcept;
	};
	inline const UrlTemplate g_DEFAULT_SYSTEM_DOWNLOAD_TEMPLATE { rcsbPdbDownloadUrlTemplate() };

	/**
	 * @brief URL that is ready to be used.
	 */
	class UrlFull
	{
	  public:
		/**
		 * @brief Use default URL template along with the systemId to form the full url
		 */
		UrlFull( const SystemId & );

		/**
		 * @brief Form the full Url using provided template along with the systemId
		 */
		UrlFull( const UrlTemplate &, const SystemId & );

		std::string str { g_DEFAULT_SYSTEM_DOWNLOAD_TEMPLATE };
		inline		operator std::string() noexcept { return str; }
		inline		operator const std::string() const noexcept { return str; }
	};
} // namespace VTX::Util::Url

#endif
