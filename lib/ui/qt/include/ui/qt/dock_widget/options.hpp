#ifndef VTX_UI_QT_DOCK_WIDGET_OPTIONS_HPP
#define VTX_UI_QT_DOCK_WIDGET_OPTIONS_HPP

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/events.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QString>
#include <util/event_hub.hpp>
#include <util/types.hpp>

class QGroupBox;
class QVBoxLayout;

namespace VTX::UI::QT::DockWidget
{

	class Options : public BaseDockWidget<Options>
	{
		/**
		 * @brief Default values.
		 */
		static constexpr bool SAVE_POWER_DEFAULT = true;

	  public:
		Options( QWidget * p_parent );
		~Options() override;

	  private:
		QPointer<QComboBox> _comboBoxTheme;
		QPointer<QComboBox> _comboBoxFont;
		QPointer<QComboBox> _comboBoxTreeViewMode;
		QPointer<QComboBox> _comboBoxSeqViewMode;
		QPointer<QComboBox> _comboBoxKBLayout;
		QPointer<QComboBox> _comboBoxColorAccessibilityMode;
		QPointer<QCheckBox> _checkBoxForceDyslexicFont;
		QPointer<QCheckBox> _checkBoxVSync;
		QPointer<QCheckBox> _checkBoxSavePower;
		QPointer<QLabel>	_labelCacheSize;
		QPointer<QLabel>	_labelHdriSize;
		QPointer<QLabel>	_labelSnapshotsSize;
		QPointer<QLabel>	_labelLogsSize;

		void		_syncThemeComboBox();
		void		_onThemeChanged( const Events::ThemeChanged & );
		QGroupBox * _createDiskUsageGroupBox();
		void		_addDiskUsageRow( QVBoxLayout *		 p_layout,
									  const QString &	 p_title,
									  const FilePath &	 p_path,
									  QPointer<QLabel> & p_labelSize );
		void		_refreshFolderInfos( const FilePath & p_path, QLabel * p_labelSize, const QString & p_title ) const;
		void		_refreshFoldersInfos();

		VTX::Util::EventHub::ScopedConnection _themeChangedConnection;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
