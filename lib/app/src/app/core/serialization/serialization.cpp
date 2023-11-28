#include "app/core/serialization/serialization.hpp"
#include "app/core/serialization/version.hpp"
#include <util/json/io.hpp>

namespace VTX::App::Core::Serialization
{
	VTX::Util::JSon::Document Serialization::readFile( const FilePath p_path ) const
	{
		const VTX::Util::JSon::Document fullDoc = Util::JSon::IO::open( p_path );

		VTX::Util::JSon::Document doc;

		if ( fullDoc.json().contains( "VERSION" ) && fullDoc.json().contains( "DOC" ) )
		{
			doc = fullDoc.json()[ "DOC" ];

			Version fileVersion;
			deserialize( fullDoc.json()[ "VERSION" ], fileVersion );

			if ( fileVersion < Version::CURRENT )
			{
				// TODO
				//  migrate( doc, fileVersion );
			}
			else if ( fileVersion > Version::CURRENT )
			{
				VTX_WARNING( "File created with a more recent version of VTX. Loading can fail." );
			}
		}
		else
		{
			doc = fullDoc;
		}

		return doc;
	}

	void Serialization::writeFile( const FilePath p_path, const VTX::Util::JSon::Document & p_doc ) const
	{
		const VTX::Util::JSon::Document versionnedDocument
			= { { "VERSION", serialize( Version::CURRENT ) }, { "DOC", p_doc } };

		Util::JSon::IO::write( p_path, versionnedDocument );
	}

} // namespace VTX::App::Core::Serialization
