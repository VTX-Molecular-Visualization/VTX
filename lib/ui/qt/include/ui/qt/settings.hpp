#ifndef __VTX_UI_QT_SETTINGS__
#define __VTX_UI_QT_SETTINGS__

#include <QSettings>
#include <app/filesystem.hpp>
#include <vector>

namespace VTX::UI::QT
{
	class ISavable;

	// TODO: set accessible for tools.
	class Settings : public QSettings
	{
	  public:
		Settings();

		inline void add( ISavable * const p_savable ) { _savables.push_back( p_savable ); }
		inline void remove( ISavable * const p_savable ) { std::erase( _savables, p_savable ); }

		void save();
		void restore() const;

	  private:
		std::vector<ISavable *> _savables;
	};

	// TODO: move to context.
	inline Settings SETTINGS;

	class ISavable
	{
	  public:
		ISavable() { SETTINGS.add( this ); }
		~ISavable() { SETTINGS.remove( this ); }

		virtual void save()	   = 0;
		virtual void restore() = 0;
	};
} // namespace VTX::UI::QT

#endif
