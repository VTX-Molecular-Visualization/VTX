#ifndef __VTX_UTIL_NETWORK__
#define __VTX_UTIL_NETWORK__

#include "logger.hpp"
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <fstream>

namespace VTX::Util::Network
{

	void httpRequest( const std::string & url )
	{
		try
		{
			asio::io_context					  ioContext;
			asio::ip::tcp::resolver				  resolver( ioContext );
			asio::ip::tcp::socket				  socket( ioContext );
			asio::ip::tcp::resolver::results_type endpoints = resolver.resolve( url, "https" );
			asio::connect( socket, endpoints );
			asio::streambuf request;
			std::ostream	requestStream( &request );
			requestStream << "GET / HTTPS/1.1\r\n";
			requestStream << "Host: " << url << "\r\n";
			requestStream << "Connection: close\r\n\r\n";
			asio::write( socket, request );

			asio::streambuf response;
			asio::read_until( socket, response, "\r\n" );

			auto data = response.data();

			std::istream responseStream( &response );
			std::string	 httpVersion;
			responseStream >> httpVersion;
			uint statusCode;
			responseStream >> statusCode;
			std::string statusMessage;
			std::getline( responseStream, statusMessage );
			if ( !responseStream || httpVersion.substr( 0, 6 ) != "HTTPS/" )
			{
				throw std::runtime_error( "invalid response" );
			}
			if ( statusCode != 200 )
			{
				throw std::runtime_error( fmt::format( " status code {}", statusCode ) );
			}
			else
			{
				asio::read_until( socket, response, "\r\n\r\n" );

				std::ofstream ofs( "test.txt", std::ios::binary );
				ofs << &response;
				ofs.close();
			}

			// Print response data.
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Network error: {}", p_e.what() );
		}
	}
} // namespace VTX::Util::Network

#endif
