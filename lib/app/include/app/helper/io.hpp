#ifndef VTX_APP_HELPER_IO_HPP
#define VTX_APP_HELPER_IO_HPP

#include "app/constants.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <util/filesystem.hpp>
#include <util/network.hpp>

namespace VTX::App::Helper::IO
{
	/**
	 * @brief Check system id.
	 */
	bool isValidSystemId( std::string_view p_systemId );

	/**
	 * @brief Build download url.
	 */
	std::string getSystemDownloadUrl(
		std::string_view p_systemId,
		std::string_view p_urlTemplate = RCSB_PDB_DOWNLOAD_URL_TEMPLATE
	);

	/**
	 * @brief Filename to write the downloaded system.
	 */
	FilePath getSystemDownloadFilename( std::string_view p_systemId );

	/**
	 * @brief Fetch a file from a remote source.
	 */
	std::optional<std::string> fetch(
		std::string_view						p_url,
		const Util::Network::ProgressCallback & p_progress = nullptr
	);

	/**
	 * @brief Write file into cache folder.
	 */
	void writeCache( std::string_view p_filename, const std::string & p_data );
} // namespace VTX::App::Helper::IO

#endif
