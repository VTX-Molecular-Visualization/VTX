#include "ui/qt/mime_type.hpp"
#include <string>
#include <util/string.hpp>

namespace VTX::UI::QT
{
	MimeType::ModelData::ModelData( const App::Core::Model::BaseModel & p_model, const DragSource & p_dragSource )
	{
		_modelID	= p_model.getId();
		_typeID		= p_model.getTypeId();
		_dragSource = p_dragSource;
	}
	MimeType::ModelData::ModelData( const QByteArray & p_data )
	{
		const std::string			   strData		= p_data.data();
		const std::vector<std::string> splittedData = Util::String::split( strData, DATA_SEPARATOR );

		const std::string modelIdStr   = splittedData[ 0 ];
		const std::string modelTypeStr = splittedData[ 1 ];
		const std::string dragSource   = splittedData[ 2 ];

		_modelID	= Util::String::strToUint( splittedData[ 0 ] );
		_typeID		= splittedData[ 1 ];
		_dragSource = DragSource( Util::String::strToUint( splittedData[ 2 ] ) );
	}
	void MimeType::ModelData::fillByteArray( QByteArray & p_byteArray ) const
	{
		const std::string modelIdStr   = std::to_string( _modelID );
		const std::string modelTypeStr = _typeID;
		const std::string dragSource   = std::to_string( int( _dragSource ) );

		p_byteArray.append( modelIdStr );
		p_byteArray.append( DATA_SEPARATOR );
		p_byteArray.append( modelTypeStr );
		p_byteArray.append( DATA_SEPARATOR );
		p_byteArray.append( dragSource );
	}

	QMimeData * const MimeType::generateMimeDataFromModel( const App::Core::Model::BaseModel & p_model,
														   const DragSource &		p_dragSource )
	{
		const ModelData data = ModelData( p_model, p_dragSource );

		QByteArray byteArray;
		data.fillByteArray( byteArray );

		QMimeData * const mimeData = new QMimeData();
		mimeData->setData( getStrMimeType( ApplicationMimeType::MODEL ), byteArray );

		return mimeData;
	}

	MimeType::ApplicationMimeType MimeType::getMimeTypeEnum( const QMimeData * const p_mimeData )
	{
		if ( p_mimeData->hasUrls() )
			return MimeType::ApplicationMimeType::FILE;
		else if ( p_mimeData->hasFormat( applicationMimeTypes[ int( MimeType::ApplicationMimeType::MODEL ) ] ) )
			return MimeType::ApplicationMimeType::MODEL;

		return MimeType::ApplicationMimeType::UNKNOWN;
	}

	bool MimeType::checkApplicationDataType( const QMimeData * const	 p_mimeData,
											 const ApplicationMimeType & p_dataType )
	{
		return p_mimeData->hasFormat( getStrMimeType( ApplicationMimeType::MODEL ) );
	}

	MimeType::ModelData MimeType::getModelData( const QMimeData * const p_mimeData )
	{
		return ModelData( p_mimeData->data( getStrMimeType( ApplicationMimeType::MODEL ) ) );
	}
} // namespace VTX::UI::QT
