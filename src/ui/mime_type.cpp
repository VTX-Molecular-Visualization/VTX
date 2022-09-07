#include "mime_type.hpp"
#include "model/base_model.hpp"
#include "model/label.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/viewpoint.hpp"
#include <string>

namespace VTX::UI
{
	const QString MimeType::applicationMimeTypes[] = {
		"application/vtx-representable",
		"application/vtx-molecule",
		"application/vtx-instantiatedrepresentation",
		"application/application/vtx-sceneitem",
		"application/vtx-basemodel",
		"application/vtx-path",
		"application/vtx-viewpoint",
		"application/vtx-label",
	};

	QMimeData * const MimeType::generateModelData( const Model::BaseModel & p_model )
	{
		const VTX::ID::VTX_ID & modelTypeID = p_model.getTypeId();
		QMimeData *				res;

		if ( modelTypeID == ID::Model::MODEL_MOLECULE || modelTypeID == ID::Model::MODEL_GENERATED_MOLECULE )
		{
			res = generateMoleculeData( static_cast<const Model::Molecule &>( p_model ) );
		}
		else if ( modelTypeID == ID::Model::MODEL_PATH )
		{
			res = generatePathData( static_cast<const Model::Path &>( p_model ) );
		}
		else if ( modelTypeID == ID::Model::MODEL_VIEWPOINT )
		{
			res = generateViewpointData( static_cast<const Model::Viewpoint &>( p_model ) );
		}
		else if ( modelTypeID == ID::Model::MODEL_LABEL )
		{
			res = generateLabelData( static_cast<const Model::Label &>( p_model ) );
		}
		else if ( modelTypeID == ID::Model::MODEL_INTANTIATED_REPRESENTATION )
		{
			res = generateInstantiatedRepresentationData(
				static_cast<const Model::Representation::InstantiatedRepresentation &>( p_model ) );
		}
		else
		{
			res = nullptr;
		}

		return res;
	}

	QMimeData * const MimeType::generateMoleculeData( const Model::Molecule & p_molecule )
	{
		QMimeData * const mimeData		= new QMimeData();
		const std::string moleculeIdStr = std::to_string( p_molecule.getId() );
		QByteArray		  byteData		= QByteArray();
		byteData.push_back( moleculeIdStr.c_str() );

		mimeData->setData( getQStringMimeType( ApplicationMimeType::BASE_MODEL ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::MOLECULE ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::SCENE_ITEM ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::REPRESENTABLE ), byteData );

		return mimeData;
	}

	QMimeData * const MimeType::generatePathData( const Model::Path & p_path )
	{
		QMimeData * const mimeData	 = new QMimeData();
		const std::string modelIdStr = std::to_string( p_path.getId() );
		QByteArray		  byteData	 = QByteArray();
		byteData.push_back( modelIdStr.c_str() );

		mimeData->setData( getQStringMimeType( ApplicationMimeType::BASE_MODEL ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::PATH ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::SCENE_ITEM ), byteData );

		return mimeData;
	}

	QMimeData * const MimeType::generateViewpointData( const Model::Viewpoint & p_viewpoint )
	{
		QMimeData * const mimeData	 = new QMimeData();
		const std::string modelIdStr = std::to_string( p_viewpoint.getId() );
		QByteArray		  byteData	 = QByteArray();
		byteData.push_back( modelIdStr.c_str() );

		mimeData->setData( getQStringMimeType( ApplicationMimeType::BASE_MODEL ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::VIEWPOINT ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::SCENE_ITEM ), byteData );

		return mimeData;
	}

	QMimeData * const MimeType::generateLabelData( const Model::Label & p_label )
	{
		QMimeData * const mimeData	 = new QMimeData();
		const std::string modelIdStr = std::to_string( p_label.getId() );
		QByteArray		  byteData	 = QByteArray();
		byteData.push_back( modelIdStr.c_str() );

		mimeData->setData( getQStringMimeType( ApplicationMimeType::BASE_MODEL ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::LABEL ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::SCENE_ITEM ), byteData );

		return mimeData;
	}

	QMimeData * const MimeType::generateInstantiatedRepresentationData(
		const Model::Representation::InstantiatedRepresentation & p_representation )
	{
		QMimeData * const mimeData		= new QMimeData();
		const std::string moleculeIdStr = std::to_string( p_representation.getId() );
		QByteArray		  byteData		= QByteArray();
		byteData.push_back( moleculeIdStr.c_str() );

		mimeData->setData( getQStringMimeType( ApplicationMimeType::BASE_MODEL ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::INSTANTIATED_REPRESENTATION ), byteData );
		mimeData->setData( getQStringMimeType( ApplicationMimeType::SCENE_ITEM ), byteData );

		return mimeData;
	}

} // namespace VTX::UI
