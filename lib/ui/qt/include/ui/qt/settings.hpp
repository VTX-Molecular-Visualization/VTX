#ifndef __VTX_UI_QT_SETTINGS__
#define __VTX_UI_QT_SETTINGS__

#include <QSettings>
#include <vector>

namespace VTX::UI::QT
{
	class ISavable;

	/**
	 * @brief Settings manager inheriting from QSettings.
	 */
	class Settings : public QSettings
	{
	  public:
		/**
		 * @brief Create settings from .ini file.
		 */
		Settings();

		/**
		 * @brief Add or remove a savable/restorable object.
		 */
		inline void add( ISavable * const p_savable ) { _savables.push_back( p_savable ); }
		inline void remove( ISavable * const p_savable ) { std::erase( _savables, p_savable ); }

		/**
		 * @brief Save or restore all registered savable/restorable objects.
		 */
		void save();
		void restore() const;

	  private:
		/**
		 * @brief Registered savable/restorable objects.
		 */
		std::vector<ISavable *> _savables;
	};

	/**
	 * @brief Interface for savable/restorable objects.
	 */
	class ISavable
	{
	  public:
		/**
		 * @brief Automatic registration on creation and unregistration on destruction.
		 */
		ISavable();
		virtual ~ISavable();

		/**
		 * @brief Save or restore settings.
		 */
		virtual void save( Settings & )			 = 0;
		virtual void restore( const Settings & ) = 0;
	};

} // namespace VTX::UI::QT

#endif
