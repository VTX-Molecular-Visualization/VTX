#ifndef ABSTRACTSCHEDULERSETTINGSWIDGET_H
#define ABSTRACTSCHEDULERSETTINGSWIDGET_H

#include <QWidget>µ
/// @brief Base interface for custom queue settings widgets.
class AbstractSchedulerSettingsWidget : public QWidget
{
	Q_OBJECT
  public:
	explicit AbstractSchedulerSettingsWidget( QWidget * parentObject = 0 );

	/// Has the GUI been modified from the current Queue state?
	bool isDirty() const { return m_isDirty; }

  signals:
	/**
	 * @brief Emitted when the options change from their initial settings.
	 */
	void modified();

  public slots:

	/// Write the information from the GUI to the Queue. Subclasses
	/// should call setDirty(false) at the end of their implementation.
	virtual void save() = 0;

	/// Update the Queue with the current configuration in the GUI. Subclasses
	/// should call setDirty(false) at the end of their implementation.
	virtual void reset() = 0;

  protected slots:

	void setDirty( bool dirty = true )
	{
		m_isDirty = dirty;
		if ( m_isDirty )
			emit modified();
	}

  protected:
	bool m_isDirty;
};

#endif // ABSTRACTSCHEDULERSETTINGSWIDGET_H
