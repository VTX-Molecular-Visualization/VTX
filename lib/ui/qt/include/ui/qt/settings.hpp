#ifndef __VTX_UI_QT_SETTINGS__
#define __VTX_UI_QT_SETTINGS__

#include <QSettings>
#include <app/filesystem.hpp>
#include <vector>

namespace VTX::UI::QT
{
	class Savable;

	// TODO: set accessible for tools.
	class Settings : public QSettings
	{
	  public:
		Settings();

		inline void add( Savable * const p_savable ) { _savables.push_back( p_savable ); }
		inline void remove( Savable * const p_savable ) { std::erase( _savables, p_savable ); }

		void save();
		void restore() const;

	  private:
		std::vector<Savable *> _savables;
	};

	inline Settings SETTINGS;

	class Savable
	{
	  public:
		Savable() { SETTINGS.add( this ); }
		~Savable() { SETTINGS.remove( this ); }

		virtual void save()	   = 0;
		virtual void restore() = 0;
	};
} // namespace VTX::UI::QT

#endif
