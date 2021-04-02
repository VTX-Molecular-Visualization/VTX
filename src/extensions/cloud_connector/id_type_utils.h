/******************************************************************************

  This source file is part of the MoleQueue project.

  Copyright 2012 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#ifndef IDTYPEUTILS_H
#define IDTYPEUTILS_H

#include "cloud_connector_global.h"
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <qjsonvalue.h>

namespace MoleQueue
{
	/// Convert an IdType to a string. This will prevent the literal value of
	/// InvalidId from being used for serialization, RPC, etc.
	inline QString idTypeToString( IdType id )
	{
		return ( id != InvalidId ) ? QString::number( id ) : QString( "Invalid" );
	}

	/// Convert a string to an IdType. This will prevent the literal value of
	/// InvalidId from being used for serialization, RPC, etc.
	inline IdType toIdType( const QString & str )
	{
		bool ok = false;
		// qlonglong = qint64
		IdType result = static_cast<IdType>( str.toLongLong( &ok ) );
		return ok ? result : InvalidId;
	}

	/// @overload
	inline IdType toIdType( const char * str ) { return toIdType( QString( str ) ); }

	/// @overload
	inline IdType toIdType( const QByteArray & str ) { return toIdType( QString( str.constData() ) ); }

	/// Convert a QJsonValue to an IdType. This will prevent the literal value of
	/// InvalidId from being used for serialization, RPC, etc.
	inline IdType toIdType( const QJsonValue & json )
	{
		// QJsonValue cannot handle integer types, only double, so round off the
		// value as a double.
		return json.isDouble() ? static_cast<IdType>( json.toDouble() + 0.5 ) : InvalidId;
	}

	/// Convert an IdType to a QJsonValue. This will prevent the literal value of
	/// InvalidId from being used for serialization, RPC, etc.
	inline QJsonValue idTypeToJson( IdType id )
	{
		return id != InvalidId ? QJsonValue( static_cast<double>( id ) ) : QJsonValue( QJsonValue::Null );
	}

	/// Convert a QVariant to an IdType. This will prevent the literal value of
	/// InvalidId from being used for serialization, RPC, etc.
	inline IdType toIdType( const QVariant & variant )
	{
		return variant.canConvert<IdType>() ? variant.value<IdType>() : InvalidId;
	}

	/// Convert an IdType to a QVariant. This will prevent the literal value of
	/// InvalidId from being used for serialization, RPC, etc.
	inline QVariant idTypeToVariant( IdType id ) { return id != InvalidId ? QVariant( id ) : QVariant(); }

} // end namespace MoleQueue

#endif // IDTYPEUTILS_H
