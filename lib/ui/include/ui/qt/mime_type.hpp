#ifndef __VTX_UI_MIME_TYPE__
#define __VTX_UI_MIME_TYPE__

#include <QByteArray>
#include <QMimeData>
#include <QString>
#include <app/old_app/id.hpp>
#include <app/core/model/base_model.hpp>

namespace VTX::UI::QT
{
	class MimeType
	{
	  private:
		inline static const char DATA_SEPARATOR = ';';

	  public:
		enum class ApplicationMimeType : int
		{
			MODEL,
			FILE,
			UNKNOWN,
		};
		inline static const QString applicationMimeTypes[] = { "application/vtx-model", "application/vtx-file" };

		enum class DragSource : int
		{
			SCENE_VIEW,
			MODEL_FIELD,
		};

		class ModelData
		{
		  public:
			ModelData( const App::Core::Model::BaseModel & p_model, const DragSource & p_dragSource );
			ModelData( const QByteArray & p_data );

			void fillByteArray( QByteArray & p_byteArray ) const;

			const App::Core::Model::ID &  getModelID() const { return _modelID; };
			const ID::VTX_ID & getTypeID() const { return _typeID; };
			const DragSource & getDragSource() const { return _dragSource; };

		  private:
			App::Core::Model::ID  _modelID;
			ID::VTX_ID _typeID;
			DragSource _dragSource;
		};

		static const QString & getStrMimeType( const ApplicationMimeType & _mimeTypeID )
		{
			return applicationMimeTypes[ int( _mimeTypeID ) ];
		}

		static QMimeData * const generateMimeDataFromModel( const App::Core::Model::BaseModel & p_model,
															const DragSource &		 p_dragSource );

		static ApplicationMimeType getMimeTypeEnum( const QMimeData * const p_mimeData );

		static bool checkApplicationDataType( const QMimeData * const	  p_mimeData,
											  const ApplicationMimeType & p_dataType );

		static ModelData getModelData( const QMimeData * const p_mimeData );
	};

} // namespace VTX::UI::QT

#endif
