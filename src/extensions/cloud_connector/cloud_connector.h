#ifndef __VTX_CLOUDMANAGER__
#define __VTX_CLOUDMANAGER__

#include "cloud_connector_global.h"
#include <QtCore/QMap>
#include <QtCore/QObject>

class Cloud;
class Server;

class CloudConnector : public QObject
{
	Q_OBJECT

  public:
	explicit CloudConnector( Server * parentServer = 0 );
	~CloudConnector();

	void readSettings();
	void writeSettings() const;

	Server *	   server() { return m_server; }
	const Server * server() const { return m_server; }

	Cloud * lookupQueue( const QString & name ) const { return clouds_list.value( name, NULL ); }

	static QStringList availableClouds();
	static bool		   cloudTypeIsValid( const QString & queueType );
	virtual Cloud *	   addCloud( const QString & cloudName, const QString & cloudType, bool replace = false );
	bool			   removeCloud( const Cloud * cloud );
	bool			   removeCloud( const QString & name );
	QList<Cloud *>	   clouds() const { return clouds_list.values(); }
	QStringList		   cloudNames() const { return clouds_list.keys(); }
	int				   numClouds() const { return clouds_list.size(); }
	CloudListType	   toCloudList() const;
	QString			   cloudConfigDirectory() const;

  public slots:
	void updateRemoteQueues() const;

  signals:
	cloudAdded( const QString & name, Cloud * queue );
	void cloudRemoved( const QString & name, Cloud * queue );
	void cloudRenamed( const QString & newName, Cloud * queue, const QString & oldName );

  protected:
	QMap<QString, Cloud *> clouds_list;
	Server *			   m_server;

  private slots:
	void cloudNameChanged( const QString & newName, const QString & oldName );
};

#endif
