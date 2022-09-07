#ifndef __VTX_UI_MIME_TYPE__
#define __VTX_UI_MIME_TYPE__

#include "id.hpp"
#include <QByteArray>
#include <QMimeData>
#include <QString>
#include <string>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
		class Molecule;
		class Path;
		class Viewpoint;
		class Label;
		namespace Representation
		{
			class InstantiatedRepresentation;
		}

	} // namespace Model

	namespace UI
	{
		class MimeType
		{
		  public:
			enum class ApplicationMimeType : int
			{
				REPRESENTABLE,
				MOLECULE,
				INSTANTIATED_REPRESENTATION,
				SCENE_ITEM,
				BASE_MODEL,
				PATH,
				VIEWPOINT,
				LABEL,
			};

			static const QString applicationMimeTypes[];

			static QString getQStringMimeType( const ApplicationMimeType & _mimeTypeID )
			{
				return applicationMimeTypes[ int( _mimeTypeID ) ];
			}

			static QMimeData * const generateModelData( const Model::BaseModel & p_model );

			static QMimeData * const generateMoleculeData( const Model::Molecule & _molecule );
			static QMimeData * const generateInstantiatedRepresentationData(
				const Model::Representation::InstantiatedRepresentation & p_representation );
			static QMimeData * const generateViewpointData( const Model::Viewpoint & p_viewpoint );
			static QMimeData * const generatePathData( const Model::Path & p_path );
			static QMimeData * const generateLabelData( const Model::Label & p_label );
		};

	} // namespace UI
} // namespace VTX

#endif
