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

#include "server.h"

#include "actionfactorymanager.h"
#include "job.h"
#include "jobmanager.h"
#include "logger.h"
#include "queue.h"
#include "queuemanager.h"
#include "pluginmanager.h"
#include "jobactionfactories/openwithactionfactory.h"

#include <molequeue/servercore/connectionlistenerfactory.h>

#include <qjsonarray.h>
#include <qjsonobject.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtCore/QStringBuilder>
#include <QtCore/QTimerEvent>

namespace MoleQueue
{

Server::Server(QObject *parentObject, QString serverName_)
  : QObject(parentObject),
    m_jobManager(new JobManager (this)),
    m_queueManager(new QueueManager (this)),
    m_jsonrpc(new JsonRpc(this)),
    m_moleQueueIdCounter(0),
    m_serverName(serverName_),
    m_jobSyncTimer(startTimer(20000)) // 20 seconds
{
  qRegisterMetaType<ConnectionListener::Error>("ConnectionListener::Error");
  qRegisterMetaType<const Job*>("const MoleQueue::Job*");
  qRegisterMetaType<QueueListType>("MoleQueue::QueueListType");

  connect(m_jsonrpc, SIGNAL(messageReceived(MoleQueue::Message)),
          SLOT(handleMessage(MoleQueue::Message)));

  connect(m_jobManager, SIGNAL(jobAboutToBeAdded(MoleQueue::Job)),
          this, SLOT(jobAboutToBeAdded(MoleQueue::Job)),
          Qt::DirectConnection);

  connect(m_jobManager, SIGNAL(jobStateChanged(const MoleQueue::Job&,
                                               MoleQueue::JobState,
                                               MoleQueue::JobState)),
          this, SLOT(dispatchJobStateChange(const MoleQueue::Job&,
                                            MoleQueue::JobState,
                                            MoleQueue::JobState)));

  connect(m_jobManager, SIGNAL(jobRemoved(MoleQueue::IdType)),
          this, SLOT(jobRemoved(MoleQueue::IdType)));

  // load the transport plugins so we know what to listen on
  PluginManager *pluginManager = PluginManager::instance();
  pluginManager->load();
}

Server::~Server()
{
  if (m_jobSyncTimer != 0)
    killTimer(m_jobSyncTimer);

  stop();

  delete m_jobManager;
  m_jobManager = NULL;

  delete m_queueManager;
  m_queueManager = NULL;
}

void Server::createConnectionListeners()
{
  PluginManager *pluginManager = PluginManager::instance();
  QList<ConnectionListenerFactory *> factories =
    pluginManager->connectionListenerFactories();

  foreach(ConnectionListenerFactory *factory, factories) {
    ConnectionListener *listener = factory->createConnectionListener(this,
                                                                     m_serverName);
    connect(listener, SIGNAL(connectionError(MoleQueue::ConnectionListener::Error,
                                             const QString&)),
            this, SIGNAL(connectionError(MoleQueue::ConnectionListener::Error,
                                             const QString&)));

    connect(listener, SIGNAL(newConnection(MoleQueue::Connection*)),
            this, SLOT(newConnectionAvailable(MoleQueue::Connection*)));

    m_connectionListeners.append(listener);
    m_jsonrpc->addConnectionListener(listener);
  }
}

void Server::readSettings(QSettings &settings)
{
  m_workingDirectoryBase = settings.value(
        "workingDirectoryBase",
        QDir::homePath() + "/.molequeue/local").toString();
  QDir dir;
  dir.mkpath(m_workingDirectoryBase);
  m_moleQueueIdCounter =
      settings.value("moleQueueIdCounter", 0).value<IdType>();

  m_queueManager->readSettings();
  const QString jobsDir = m_workingDirectoryBase + "/jobs";
  dir.mkpath(jobsDir);
  m_jobManager->loadJobState(jobsDir);
}

void Server::writeSettings(QSettings &settings) const
{
  settings.setValue("workingDirectoryBase", m_workingDirectoryBase);
  settings.setValue("moleQueueIdCounter", m_moleQueueIdCounter);

  m_queueManager->writeSettings();
  m_jobManager->syncJobState();
}

void Server::start()
{
  if(m_connectionListeners.empty())
    createConnectionListeners();

  foreach (ConnectionListener *listener, m_connectionListeners) {
    listener->start();
  }

  Logger::logDebugMessage(tr("Server started listening on address '%1'")
                          .arg(m_serverName));
}

void Server::forceStart()
{
  // Force stop and restart
  stop(true);
  start();
}

void Server::stop(bool force) {

  foreach (Connection *conn, m_connections) {
    conn->close();
    delete conn;
  }

  foreach (ConnectionListener *listener, m_connectionListeners) {
    listener->stop(force);
    delete listener;
  }

  m_connections.clear();
  m_connectionListeners.clear();

}

void Server::stop() {
  stop(false);
}

void Server::dispatchJobStateChange(const Job &job, JobState oldState,
                                    JobState newState)
{
  Connection *connection = m_connectionLUT.value(job.moleQueueId());
  EndpointIdType endpoint = m_endpointLUT.value(job.moleQueueId());

  if (connection == NULL)
    return;

  Message msg(Message::Notification, connection, endpoint);
  msg.setMethod("jobStateChanged");
  QJsonObject paramsObject;
  paramsObject.insert("moleQueueId", idTypeToJson(job.moleQueueId()));
  paramsObject.insert("oldState", QString(jobStateToString(oldState)));
  paramsObject.insert("newState", QString(jobStateToString(newState)));
  msg.setParams(paramsObject);
  msg.send();
}

void Server::jobAboutToBeAdded(Job job)
{
  IdType nextMoleQueueId = ++m_moleQueueIdCounter;

  QSettings settings;
  settings.setValue("moleQueueIdCounter", m_moleQueueIdCounter);

  job.setMoleQueueId(nextMoleQueueId);
  job.setLocalWorkingDirectory(m_workingDirectoryBase + "/jobs/" +
                                idTypeToString(nextMoleQueueId));

  // If the outputDirectory is blank, set it now
  if (job.outputDirectory().isEmpty())
    job.setOutputDirectory(job.localWorkingDirectory());

  // Create the local working directory
  if (job.localWorkingDirectory().isEmpty() ||
      !QDir().mkpath(job.localWorkingDirectory())) {
    Logger::logError(tr("Error creating working directory for job %1 "
                        "(dir='%2')").arg(idTypeToString(job.moleQueueId()))
                     .arg(job.localWorkingDirectory()),
                     job.moleQueueId());
  }
}

void Server::newConnectionAvailable(Connection *connection)
{
  m_connections.append(connection);
  connect(connection, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

  Logger::logDebugMessage(tr("Client connected: %1")
                          .arg(connection->connectionString()));
}

void Server::clientDisconnected()
{
  Connection *conn = qobject_cast<Connection*>(sender());
  if (conn == NULL)
    return;

  Logger::logDebugMessage(tr("Client disconnected: %1")
                          .arg(conn->connectionString()));

  m_connections.removeOne(conn);

  // Remove connection from look up table and any endpoints key on molequeueids
  // associated with that connection.
  QList<IdType> moleQueueIds = m_connectionLUT.keys(conn);

  foreach(IdType moleQueueId, moleQueueIds) {
    m_connectionLUT.remove(moleQueueId);
    m_endpointLUT.remove(moleQueueId);
  }

  conn->deleteLater();
}

void Server::jobRemoved(MoleQueue::IdType moleQueueId)
{
  m_connectionLUT.remove(moleQueueId);
  m_endpointLUT.remove(moleQueueId);
}

void Server::handleMessage(const Message &message)
{
  switch (message.type()) {
  case Message::Request:
    handleRequest(message);
    break;
  default:
    Logger::logDebugMessage(tr("Unhandled message; no handler for type: %1\n%2")
                            .arg(message.type())
                            .arg(QString(message.toJson())));
    break;
  }
}

void Server::handleRequest(const Message &message)
{
  const QString method = message.method();
  if (method == "listQueues")
    handleListQueuesRequest(message);
  else if (method == "submitJob")
    handleSubmitJobRequest(message);
  else if (method == "cancelJob")
    handleCancelJobRequest(message);
  else if (method == "lookupJob")
    handleLookupJobRequest(message);
  else if (method == "registerOpenWith")
    handleRegisterOpenWithRequest(message);
  else if (method == "listOpenWithNames")
    handleListOpenWithNamesRequest(message);
  else if (method == "unregisterOpenWith")
    handleUnregisterOpenWithRequest(message);
  else if (method == "rpcKill")
    handleRpcKillRequest(message);
  else
    handleUnknownMethod(message);
}

void Server::handleUnknownMethod(const Message &message)
{
  Message errorMessage = message.generateErrorResponse();
  errorMessage.setErrorCode(-32601);
  errorMessage.setErrorMessage("Method not found");
  QJsonObject errorDataObject;
  errorDataObject.insert("request", message.toJsonObject());
  errorMessage.setErrorData(errorDataObject);
  errorMessage.send();

  Logger::logDebugMessage(
        tr("Received JSON-RPC request with invalid method '%1':\n%2")
        .arg(message.method()).arg(QString(message.toJson())));
}

void Server::handleInvalidParams(const Message &message,
                                 const QString &description)
{
  Message errorMessage = message.generateErrorResponse();
  errorMessage.setErrorCode(-32602);
  errorMessage.setErrorMessage("Invalid params");
  QJsonObject errorDataObject;
  errorDataObject.insert("description", description);
  errorDataObject.insert("request", message.toJsonObject());
  errorMessage.setErrorData(errorDataObject);
  errorMessage.send();

  Logger::logDebugMessage(
        tr("Received JSON-RPC request with invalid parameters (%1):\n%2")
        .arg(description).arg(QString(message.toJson())));
}

void Server::handleListQueuesRequest(const Message &message)
{
  // Build result object (queue list)
  QueueListType queueList = m_queueManager->toQueueList();
  QJsonObject jsonQueueList;
  foreach (QString queueName, queueList.keys()) {
    jsonQueueList.insert(queueName,
                         QJsonArray::fromStringList(queueList[queueName]));
  }

  // Create response message
  Message response = message.generateResponse();
  response.setResult(jsonQueueList);
  response.send();
}

void Server::handleSubmitJobRequest(const Message &message)
{
  // Validate params -- are the params an object?
  if (!message.params().isObject()) {
    handleInvalidParams(message, "submitJob params member must be an object.");
    return;
  }

  QJsonObject paramsObject = message.params().toObject();

  // Are the required queue and program members present?
  if (!paramsObject.contains("queue")) {
    handleInvalidParams(message, "Required params.queue member missing.");
    return;
  }
  if (!paramsObject.contains("program")) {
    handleInvalidParams(message, "Required params.program member missing.");
    return;
  }
  if (!paramsObject.value("queue").isString()) {
    handleInvalidParams(message, "params.queue member must be a string.");
    return;
  }
  if (!paramsObject.value("program").isString()) {
    handleInvalidParams(message, "params.program member must be a string.");
    return;
  }

  // Do the queue and program exist?
  QString queueString = paramsObject.value("queue").toString();
  QString programString = paramsObject.value("program").toString();
  Queue *queue = m_queueManager->lookupQueue(queueString);
  if (!queue) {
    Message errorMessage = message.generateErrorResponse();
    errorMessage.setErrorCode(MoleQueue::InvalidQueue);
    errorMessage.setErrorMessage("Invalid queue");
    QJsonObject errorDataObject;
    errorDataObject.insert("queue", queueString);
    QJsonArray validQueues =
        QJsonArray::fromStringList(m_queueManager->queueNames());
    errorDataObject.insert("valid queues", validQueues);
    errorDataObject.insert("request", message.toJsonObject());
    errorMessage.setErrorData(errorDataObject);
    errorMessage.send();

    Logger::logDebugMessage(
          tr("Received submitJob request with invalid queue (%1):\n%2")
          .arg(queueString).arg(QString(message.toJson())));
    return;
  }
  Program *program = queue->lookupProgram(programString);
  if (!program) {
    Message errorMessage = message.generateErrorResponse();
    errorMessage.setErrorCode(MoleQueue::InvalidProgram);
    errorMessage.setErrorMessage("Invalid program");
    QJsonObject errorDataObject;
    errorDataObject.insert("program", programString);
    QJsonArray validPrograms =
        QJsonArray::fromStringList(queue->programNames());
    errorDataObject.insert("valid programs for queue", validPrograms);
    errorDataObject.insert("request", message.toJsonObject());
    errorMessage.setErrorData(errorDataObject);
    errorMessage.send();

    Logger::logDebugMessage(
          tr("Received submitJob request with invalid program (%1/%2):\n%3")
          .arg(queueString).arg(programString).arg(QString(message.toJson())));
    return;
  }

  // Everything checks out -- Create the job and send the response.
  Job job = m_jobManager->newJob(paramsObject);
  Logger::logDebugMessage(tr("Job submission requested:\n%1")
                          .arg(QString(message.toJson())), job.moleQueueId());

  Message response = message.generateResponse();
  QJsonObject resultObject;
  resultObject.insert("moleQueueId", idTypeToJson(job.moleQueueId()));
  resultObject.insert("workingDirectory", job.localWorkingDirectory());
  response.setResult(resultObject);
  response.send();

  m_connectionLUT.insert(job.moleQueueId(), message.connection());
  m_endpointLUT.insert(job.moleQueueId(), message.endpoint());

  // Submit the job after sending the response -- otherwise the client can
  // receive job state change notifications for a job before knowing its
  // MoleQueueId...
  queue->submitJob(job);
}

void Server::handleCancelJobRequest(const Message &message)
{
  // Validate request
  if (!message.params().isObject()) {
    handleInvalidParams(message, "cancelJob params member must be an object.");
    return;
  }

  QJsonObject paramsObject = message.params().toObject();

  // Is the required moleQueueId member present?
  if (!paramsObject.contains("moleQueueId")) {
    handleInvalidParams(message, "Required params.moleQueueId member missing.");
    return;
  }

  // Is the required moleQueueId member valid?
  IdType moleQueueId = toIdType(paramsObject.value("moleQueueId"));
  Job job = m_jobManager->lookupJobByMoleQueueId(moleQueueId);
  if (!job.isValid()) {
    Message errorMessage = message.generateErrorResponse();
    errorMessage.setErrorCode(MoleQueue::InvalidMoleQueueId);
    errorMessage.setErrorMessage("Unknown MoleQueue ID");
    QJsonObject errorDataObject;
    errorDataObject.insert("moleQueueId", paramsObject.value("moleQueueId"));
    errorMessage.setErrorData(errorDataObject);
    errorMessage.send();

    Logger::logDebugMessage(
          tr("Received cancelJob request with invalid MoleQueue ID (%1):\n%2")
          .arg(idTypeToString(moleQueueId)).arg(QString(message.toJson())),
          moleQueueId);
    return;
  }

  // Is the job in a state that it can be canceled?
  JobState state = job.jobState();
  bool stateValid = false;
  switch (state) {
  case MoleQueue::Accepted:
  case MoleQueue::QueuedLocal:
  case MoleQueue::Submitted:
  case MoleQueue::QueuedRemote:
  case MoleQueue::RunningLocal:
  case MoleQueue::RunningRemote:
    stateValid = true;
  default:
    break;
  }

  if (!stateValid) {
    Message errorMessage = message.generateErrorResponse();
    errorMessage.setErrorCode(MoleQueue::InvalidJobState);
    errorMessage.setErrorMessage("Cannot cancel job: Job not running.");
    QJsonObject errorDataObject;
    errorDataObject.insert("moleQueueId", paramsObject.value("moleQueueId"));
    errorDataObject.insert("jobState", QLatin1String(jobStateToString(state)));
    errorMessage.setErrorData(errorDataObject);
    errorMessage.send();

    Logger::logDebugMessage(
          tr("Received cancelJob request for non-running job (%1, %2):\n%3")
          .arg(idTypeToString(moleQueueId))
          .arg(jobStateToGuiString(state))
          .arg(QString(message.toJson())),
          moleQueueId);
    return;
  }

  Queue *queue = m_queueManager->lookupQueue(job.queue());
  if (!queue) {
    Message errorMessage = message.generateErrorResponse();
    errorMessage.setErrorCode(MoleQueue::InvalidQueue);
    errorMessage.setErrorMessage("Queue no longer exists");
    QJsonObject errorDataObject;
    errorDataObject.insert("moleQueueId", paramsObject.value("moleQueueId"));
    errorDataObject.insert("queue", job.queue());
    errorMessage.setErrorData(errorDataObject);
    errorMessage.send();

    Logger::logDebugMessage(
          tr("Received cancelJob request for deleted queue (%1, %2):\n%3")
          .arg(idTypeToString(moleQueueId))
          .arg(job.queue())
          .arg(QString(message.toJson())));
    return;
  }

  queue->killJob(job);

  Message response = message.generateResponse();
  QJsonObject resultObject;
  resultObject.insert("moleQueueId", idTypeToJson(moleQueueId));
  response.setResult(resultObject);
  response.send();
}

void Server::handleLookupJobRequest(const Message &message)
{
  // Validate request
  if (!message.params().isObject()) {
    handleInvalidParams(message, "lookupJob params member must be an object.");
    return;
  }

  QJsonObject paramsObject = message.params().toObject();

  // Is the required moleQueueId member present?
  if (!paramsObject.contains("moleQueueId")) {
    handleInvalidParams(message, "Required params.moleQueueId member missing.");
    return;
  }

  // Is the required moleQueueId member valid?
  IdType moleQueueId = toIdType(paramsObject.value("moleQueueId"));
  Job job = m_jobManager->lookupJobByMoleQueueId(moleQueueId);
  if (!job.isValid()) {
    Message errorMessage = message.generateErrorResponse();
    errorMessage.setErrorCode(MoleQueue::InvalidMoleQueueId);
    errorMessage.setErrorMessage("Unknown MoleQueue ID");
    QJsonObject errorDataObject;
    errorDataObject.insert("moleQueueId", paramsObject.value("moleQueueId"));
    errorMessage.setErrorData(errorDataObject);
    errorMessage.send();

    Logger::logDebugMessage(
          tr("Received lookupJob request with invalid MoleQueue ID (%1):\n%2")
          .arg(idTypeToString(moleQueueId)).arg(QString(message.toJson())),
          moleQueueId);
    return;
  }

  // Send reply
  Message response = message.generateResponse();
  response.setResult(job.toJsonObject());
  response.send();
}

void Server::handleRegisterOpenWithRequest(const Message &message)
{
  // validate request
  if (!message.params().isObject()) {
    handleInvalidParams(message,
                        "registerOpenWith params member must be an object.");
    return;
  }

  QJsonObject paramsObject = message.params().toObject();

  // At a minimum, name and method must be specified:
  if (!paramsObject["name"].isString() ||
      !paramsObject["method"].isObject()) {
    handleInvalidParams(message, "\"params.name\" (string) and "
                        "\"params.method\" (object) must both be present.");
    return;
  }

  const QString name(paramsObject["name"].toString());
  const QJsonObject methodObject(paramsObject["method"].toObject());

  OpenWithActionFactory::HandlerType handlerType;
  QString executable;
  QString rpcServer;
  QString rpcMethod;

  if (methodObject["executable"].isString()) {
    handlerType = OpenWithActionFactory::ExecutableHandler;
    executable = methodObject["executable"].toString();
  }
  else if (methodObject["rpcServer"].isString() &&
           methodObject["rpcMethod"].isString()) {
    handlerType = OpenWithActionFactory::RpcHandler;
    rpcServer = methodObject["rpcServer"].toString();
    rpcMethod = methodObject["rpcMethod"].toString();
  }
  else {
    handleInvalidParams(message, "\"params.method\" invalid.");
    return;
  }

  if (name.isEmpty()) {
    handleInvalidParams(message, "\"params.name\" must be a non-empty string.");
    return;
  }

  // Validate and extract patterns.
  QList<QRegExp> patterns;
  if (paramsObject.contains("patterns")) {
    if (!paramsObject["patterns"].isArray()) {
      handleInvalidParams(message, "\"params.patterns\" member must be a JSON "
                          "array.");
      return;
    }

    foreach (const QJsonValue &pattern, paramsObject["patterns"].toArray()) {
      if (!pattern.isObject()) {
        handleInvalidParams(message, "\"params.patterns\" array entries must "
                            "be JSON objects.");
        return;
      }

      const QJsonObject patternObject = pattern.toObject();
      QRegExp regexp;
      if (patternObject["regexp"].isString()) {
        regexp.setPatternSyntax(QRegExp::RegExp2);
        regexp.setPattern(patternObject["regexp"].toString());
      }
      else if (patternObject["wildcard"].isString()) {
        regexp.setPatternSyntax(QRegExp::WildcardUnix);
        regexp.setPattern(patternObject["wildcard"].toString());
      }
      else {
        handleInvalidParams(message, "\"params.patterns\" contains an entry "
                            "that is not a regexp or wildcard.");
        return;
      }

      if (patternObject.contains("caseSensitive")) {
        bool caseSensitive(patternObject.value("caseSensitive").toBool(true));
        regexp.setCaseSensitivity(caseSensitive ? Qt::CaseSensitive
                                                : Qt::CaseInsensitive);
      }

      patterns << regexp;
    }
  }

  // If no patterns are specified, match all files:
  if (patterns.empty())
    patterns << QRegExp("*", Qt::CaseSensitive, QRegExp::WildcardUnix);

  // Get existing open-with handlers
  ActionFactoryManager *afm = ActionFactoryManager::instance();
  QList<OpenWithActionFactory*> factories =
      afm->factoriesOfType<OpenWithActionFactory>();

  // Check for name conflicts:
  foreach (const OpenWithActionFactory *factory, factories) {
    if (factory->name() == name) {
      Message error = message.generateErrorResponse();
      error.setErrorCode(1);
      error.setErrorMessage(
            QLatin1Literal("Name conflict: An open-with handler named '") % name
            % QLatin1Literal("' already exists."));
      error.send();
      return;
    }
  }

  // Create a new handler:
  OpenWithActionFactory *newFactory(new OpenWithActionFactory);
  newFactory->setName(name);
  newFactory->setFilePatterns(patterns);

  switch (handlerType) {
  case OpenWithActionFactory::ExecutableHandler:
    newFactory->setExecutable(executable);
    break;
  case OpenWithActionFactory::RpcHandler:
    newFactory->setRpcDetails(rpcServer, rpcMethod);
    break;
  default:
  case OpenWithActionFactory::NoHandler:
    break;
  }

  afm->addFactory(newFactory);

  Message response = message.generateResponse();
  response.setResult(QLatin1String("success"));
  response.send();
}

void Server::handleListOpenWithNamesRequest(const Message &message)
{
  // Build result object
  ActionFactoryManager *afm = ActionFactoryManager::instance();
  QList<OpenWithActionFactory*> handlers(
        afm->factoriesOfType<OpenWithActionFactory>());
  QJsonArray result;
  foreach (OpenWithActionFactory *handler, handlers)
    result.append(handler->name());

  // Create response message
  Message response = message.generateResponse();
  response.setResult(result);
  response.send();
}

void Server::handleUnregisterOpenWithRequest(const Message &message)
{
  // Validate
  if (!message.params().isObject()) {
    handleInvalidParams(message, "params value must be an object.");
    return;
  }

  QJsonObject paramsObject(message.params().toObject());
  if (!paramsObject["name"].isString()) {
    handleInvalidParams(message, "\"params.name\" value must be a string.");
    return;
  }

  QString handlerName(paramsObject["name"].toString());

  // Search for matching handler
  ActionFactoryManager *afm = ActionFactoryManager::instance();
  QList<OpenWithActionFactory*> handlers =
      afm->factoriesOfType<OpenWithActionFactory>();
  OpenWithActionFactory *handler;
  foreach (OpenWithActionFactory *h, handlers) {
    if (handlerName == h->name()) {
      handler = h;
      break;
    }
  }

  if (!handler) {
    Message error = message.generateErrorResponse();
    error.setErrorCode(1);
    error.setErrorMessage(QString("File handler '%1'' not found!")
                          .arg(handlerName));
    error.send();
    return;
  }

  // Remove handler
  afm->removeFactory(handler);

  // Send response
  Message response = message.generateResponse();
  response.setResult(QLatin1String("success"));
  response.send();
}

void Server::handleRpcKillRequest(const Message &message)
{
  QSettings settings;
  bool enabled = settings.value("enableRpcKill", false).toBool();

  Message response = message.generateResponse();
  QJsonObject resultObject;
  resultObject.insert("success", enabled);
  response.setResult(resultObject);
  response.send();

  if (enabled) {
    qApp->processEvents(QEventLoop::AllEvents, 1000);
    qApp->quit();
  }
}

void Server::timerEvent(QTimerEvent *e)
{
  if (e->timerId() == m_jobSyncTimer) {
    e->accept();
    m_jobManager->syncJobState();
    return;
  }

  QObject::timerEvent(e);
}

} // end namespace MoleQueue
