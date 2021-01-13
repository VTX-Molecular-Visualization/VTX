#include "mime_type.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include <string>

namespace VTX
{
	namespace UI
	{
		const QString MimeType::applicationMimeTypes[] = {
			"application/vtx-representable",		 "application/vtx-molecule",  "application/vtx-instantiatedrepresentation",
			"application/application/vtx-sceneitem", "application/vtx-basemodel",
		};

		QMimeData * const MimeType::generateMoleculeData( const Model::Molecule & p_molecule )
		{
			QMimeData * const mimeData	  	= new QMimeData();
			const std::string moleculeIdStr = std::to_string( p_molecule.getId() );
			QByteArray	byteData	  		= QByteArray();
			byteData.push_back( moleculeIdStr.c_str() );

			mimeData->setData( getQStringMimeType( ApplicationMimeType::BASE_MODEL ), byteData );
			mimeData->setData( getQStringMimeType( ApplicationMimeType::MOLECULE ), byteData );
			mimeData->setData( getQStringMimeType( ApplicationMimeType::SCENE_ITEM ), byteData );
			mimeData->setData( getQStringMimeType( ApplicationMimeType::REPRESENTABLE ), byteData );

			return mimeData;
		}

		QMimeData * const MimeType::generateInstantiatedRepresentationData( const Model::Representation::InstantiatedRepresentation & p_representation )
		{
			QMimeData * const mimeData	    = new QMimeData();
			const std::string moleculeIdStr = std::to_string( p_representation.getId() );
			QByteArray	byteData	 		= QByteArray();
			byteData.push_back( moleculeIdStr.c_str() );

			mimeData->setData( getQStringMimeType( ApplicationMimeType::BASE_MODEL ), byteData );
			mimeData->setData( getQStringMimeType( ApplicationMimeType::INSTANTIATED_REPRESENTATION ), byteData );
			mimeData->setData( getQStringMimeType( ApplicationMimeType::SCENE_ITEM ), byteData );

			return mimeData;
		}

	} // namespace UI
} // namespace VTX
