#include "app/old/internal/setting/register_map.hpp"

namespace VTX::App::Old::Internal::Setting
{
	// TODO reimplement that without Qt
	std::string RegisterMap::getStringValue( const std::string & p_key, const std::string & p_default )
	{
		return "";
		// const QSettings settings( QSettings::Format::NativeFormat,
		//						  QSettings::Scope::UserScope,
		//						  QString::fromStdString( VTX_PROJECT_NAME ),
		//						  QString::fromStdString( VTX_PROJECT_NAME ) );

		// return settings.value( p_key, p_default ).toString().toStdString();
	}
	void RegisterMap::setStringValue( const std::string & p_key, const std::string & p_value )
	{
		// QSettings settings( QSettings::Format::NativeFormat,
		//					QSettings::Scope::UserScope,
		//					QString::fromStdString( VTX_PROJECT_NAME ),
		//					QString::fromStdString( VTX_PROJECT_NAME ) );

		// settings.setValue( QString::fromStdString( p_key ), p_value );
	}
	bool RegisterMap::containKey( const std::string & p_key )
	{
		return false;

		// QSettings settings( QSettings::Format::NativeFormat,
		//					QSettings::Scope::UserScope,
		//					QString::fromStdString( VTX_PROJECT_NAME ),
		//					QString::fromStdString( VTX_PROJECT_NAME ) );

		// return settings.contains( QString::fromStdString( p_key ) );
	}

} // namespace VTX::App::Old::Internal::Setting
