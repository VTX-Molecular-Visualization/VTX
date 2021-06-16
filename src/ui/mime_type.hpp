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
		class Molecule;
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
			enum class ApplicationMimeType
			{
				REPRESENTABLE,
				MOLECULE,
				INSTANTIATED_REPRESENTATION,
				SCENE_ITEM,
				BASE_MODEL,
			};

			static const QString applicationMimeTypes[];

			static QString getQStringMimeType( const ApplicationMimeType & _mimeTypeID ) { return applicationMimeTypes[ int( _mimeTypeID ) ]; }

			static QMimeData * const generateMoleculeData( const Model::Molecule & _molecule );
			static QMimeData * const generateInstantiatedRepresentationData( const Model::Representation::InstantiatedRepresentation & p_representation );
		};

	} // namespace UI
} // namespace VTX

#endif
