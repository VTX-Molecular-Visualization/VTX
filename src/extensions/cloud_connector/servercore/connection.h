#ifndef CLOUD_CONNECTOR_CONNECTION_H
#define CLOUD_CONNECTOR_CONNECTION_H

#include "servercoreglobal.h"
#include <QObject>

/**
 * @class Connection connection.h <molequeue/servercore/connection.h>
 * @brief The Connection class is an interface defining the connection using to
 * communicate between MoleQueue processes. Subclasses provide concrete
 * implements for example based on local socket @see LocalSocketConnection
 */
class MOLEQUEUESERVERCORE_EXPORT Connection : public QObject
{
	Q_OBJECT
  public:
	/**
	 * Constructor.
	 *
	 * @param parentObject parent
	 */
	explicit Connection( QObject * parentObject = 0 ) : QObject( parentObject ) {}

	/**
	 * Open the connection
	 */
	virtual void open() = 0;

	/**
	 * Start receiving messages on this connection
	 */
	virtual void start() = 0;

	/**
	 * Close the connection. Once a conneciton is closed if can't reused.
	 */
	virtual void close() = 0;

	/*
	 * @return true, if the connection is open ( open has been called,
	 * false otherwise
	 */
	virtual bool isOpen() = 0;

	/**
	 * @return the connect string description the endpoint the connection is
	 * connected to.
	 */
	virtual QString connectionString() const = 0;

	/**
	 * Send the @a packet on the connection to @a endpoint.
	 */
	virtual bool send( const PacketType & packet, const EndpointIdType & endpoint ) = 0;

	/**
	 * Flush all pending messages to the other endpoint.
	 */
	virtual void flush() = 0;

  signals:
	/**
	 * Emitted when a new message has been received on this connection.
	 *
	 * @param msg The message received.
	 */
	void packetReceived( const MoleQueue::PacketType & packet, const MoleQueue::EndpointIdType & endpoint );

	/**
	 * Emited when the connection is disconnected.
	 */
	void disconnected();
};

#endif // MOLEQUEUE_CONNECTION_H
