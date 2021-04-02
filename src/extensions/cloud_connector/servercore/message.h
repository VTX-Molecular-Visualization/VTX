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

#ifndef MOLEQUEUE_MESSAGE_H
#define MOLEQUEUE_MESSAGE_H

#include "molequeueservercoreexport.h"
#include "servercoreglobal.h"

#include <QtCore/QJsonObject>
#include <QtCore/QByteArray>
#include <QtCore/QFlags>
#include <QtCore/QString>

class MessageTest;
class ServerTest;

namespace MoleQueue {
class Connection;
class JsonRpc;

/**
 * @class Message message.h <molequeue/servercore/message.h>
 * @brief The Message class encaspulates a single JSON-RPC transmission.
 * @author David C. Lonie
 *
 * The Message class provides an interface to construct, interpret, and
 * manipulate JSON-RPC messages.
 *
 * There are four types of valid JSON-RPC messages: Requests, notifications,
 * responses, and errors. The type() method can be used to determine a given
 * Message's MessageType. A subset of the Message API is valid for each type;
 * the allowed attributes are dependent on the message type:
 *
 * - Request
 *   - id
 *   - method
 *   - params
 * - Notification
 *   - method
 *   - params
 * - Response
 *   - id
 *   - method (may be empty -- used only for convenience, not part of JSON-RPC
 *     specification).
 *   - result
 * - Error
 *   - id
 *   - method (may be empty -- used only for convenience, not part of JSON-RPC
 *     specification).
 *   - errorCode
 *   - errorMessage
 *   - errorData
 *
 * Attempting to access an attribute that is invalid for the current type
 * will cause a warning to be printed and a default-constructed value is
 * returned.
 *
 * A Message may be constructed from a QJsonObject by using the QJsonObject
 * constructor and calling parse(). See the parse() documentation for more
 * details.
 *
 * When handling a Request Message, the generateResponse() and
 * generateErrorResponse() methods may be used to easily construct an empty
 * reply with the method, id, connection, and endpoint of the request.
 *
 * Once a message is ready to send, call the send() method. This will assign
 * and set a unique id to outgoing requests and call Connection::send() with a
 * JSON representation of the Message. If the application needs to track the id
 * of a request in order to identify the reply, record the id after calling
 * send().
 *
 * The Request ids and methods are stored in an internal lookup table upon
 * sending. This is used to set the method of the incoming reply. If the lookup
 * fails, the message will be parsed properly, but the method attribute will not
 * be set.
 *
 * The JSON representation can be generated
 * and obtained by calling toJson(), and a QJsonObject representation is
 * available from the toJsonObject() method.
 */
class MOLEQUEUESERVERCORE_EXPORT Message
{
public:
  // Used for unit testing:
  friend class ::MessageTest;
  friend class ::ServerTest;

  /// Flags representing different types of JSON-RPC messages
  enum MessageType {
    /// A JSON-RPC request, with id, method, and params attributes.
    Request = 0x1,
    /// A JSON-RPC notification, with method and params attributes.
    Notification = 0x2,
    /// A JSON-RPC response, with id, method, and result attributes.
    Response = 0x4,
    /// A JSON-RPC error, with id, method, and errorCode, errorMessage, and
    /// errorData attributes.
    Error = 0x8,
    /// This MessageType indicates that this Message holds a raw QJsonObject
    /// that has not been interpreted. Call parse() to convert this Message
    /// into an appropriate type.
    Raw = 0x10,
    /// This Message is invalid.
    Invalid = 0x20
  };
  Q_DECLARE_FLAGS(MessageTypes, MessageType)

  /// Construct an Invalid Message using the @a conn and @a endpoint_.
  Message(Connection *conn = NULL, EndpointIdType endpoint_ = EndpointIdType());

  /// Construct an empty Message with the specified @a type that uses the
  /// @a conn and @a endpoint_.
  Message(MessageType type_, Connection *conn = NULL,
          EndpointIdType endpoint_ = EndpointIdType());

  /// Construct a Raw Message with the specified @a type that uses the
  /// @a conn and @a endpoint_. The @a rawJson QJsonObject will be cached to be
  /// parsed by parse() later.
  Message(const QJsonObject &rawJson, Connection *conn = NULL,
          EndpointIdType endpoint_ = EndpointIdType());

  /// Copy constructor
  Message(const Message &other);

  /// Assignment operator
  Message &operator=(const Message &other);

  /// @return The MessageType of this Message.
  MessageType type() const;

  /**
   * @{
   * The name of the method used in the remote procedure call.
   * @note This function is only valid for Request, Notification, Response, and
   * Error messages.
   */
  QString method() const;
  void setMethod(const QString &m);
  /**@}*/

  /**
   * @{
   * The parameters used in the remote procedure call.
   * @note This function is only valid for Request and Notification messages.
   */
  QJsonValue params() const;
  QJsonValue& paramsRef();
  void setParams(const QJsonArray &p);
  void setParams(const QJsonObject &p);
  /**@}*/

  /**
   * @{
   * The result object used in a remote procedure call response.
   * @note This function is only valid for Response messages.
   */
  QJsonValue result() const;
  QJsonValue& resultRef();
  void setResult(const QJsonValue &r);
  /**@}*/

  /**
   * @{
   * The integral error code used in a remote procedure call error response.
   * @note This function is only valid for Error messages.
   */
  int errorCode() const;
  void setErrorCode(int e);
  /**@}*/

  /**
   * @{
   * The error message string used in a remote procedure call error response.
   * @note This function is only valid for Error messages.
   */
  QString errorMessage() const;
  void setErrorMessage(const QString &e);
  /**@}*/

  /**
   * @{
   * The data object used in a remote procedure call error response.
   * @note This function is only valid for Error messages.
   */
  QJsonValue errorData() const;
  QJsonValue& errorDataRef();
  void setErrorData(const QJsonValue &e);
  /**@}*/

  /**
   * @{
   * The message id used in a remote procedure call.
   * @note This function is only valid for Request, Response, and Error
   * messages.
   */
  MessageIdType id() const;
protected: // Users should have no reason to set this:
  void setId(const MessageIdType &i);
public:
  /**@}*/

  /**
   * @{
   * The connection associated with the remote procedure call.
   */
  Connection* connection() const;
  void setConnection(Connection* c);
  /**@}*/

  /**
   * @{
   * The connection endpoint associated with the remote procedure call.
   */
  EndpointIdType endpoint() const;
  void setEndpoint(const EndpointIdType &e);
  /**@}*/

  /**
   * @return A QJsonObject representation of the remote procedure call.
   */
  QJsonObject toJsonObject() const;

  /**
   * @return A string representation of the remote procedure call.
   */
  PacketType toJson() const;

  /**
   * @brief Send the message to the associated connection and endpoint.
   * @return True on success, false on failure.
   * @note If this message is a Request, a unique id will be assigned prior to
   * sending. Use the id() method to retrieve the assigned id. The id is
   * registered internally to properly identify the peer's Response or Error
   * message.
   */
  bool send();

  /**
   * @brief Create a new Response message in reply to a
   * Request. The connection, endpoint, id, and method will be copied from
   * @a this Message.
   * @note This function is only valid for Request messages.
   */
  Message generateResponse() const;

  /**
   * @brief Create a new Error message in reply to a Request.
   * The connection, endpoint, id, and method will be copied from @a this
   * Message.
   * @note This function is only valid for Request, Raw, and Invalid messages.
   */
  Message generateErrorResponse() const;

  /**
   * @{
   * @brief Interpret the raw QJsonObject passed to the constructor that
   * takes a QJsonObject argument.
   * @return True on success, false on failure.
   * @note This function is only valid for Raw messages.
   *
   * This function will intepret the string as JSON, detect the type of message,
   * and update this message's type, and populate the internal data structures.
   *
   * The function returns true if the message was successfully interpreted, and
   * false if any error occured during parsing/interpretation. If any errors
   * occurred, the optional Message reference argument will be overwritten with
   * an appropriate error response. The following JSON-RPC 2.0 standard errors
   * are detected:
   *
   * - -32600 Invalid request
   *   - The message type could not be determined.
   *
   * The JsonRpc class will handle the following errors as message are received:
   *
   * - -32700 Parse error
   *   - Invalid JSON received, an error occurred during parsing.
   * - -32603 Internal error
   *   - Internal JSON-RPC error
   *
   * The remaining standard JSON-RPC error codes should be handled by the
   * application developer:
   *
   * - -32601 Method not found
   *   - Method not supported by application
   * - -32602 Invalid params
   *   - Inappropriate parameters supplied for requested method.
   *
   * This method is intended to be used as follows:
   @code
   QJsonObject jsonObject = ...;
   Message message(jsonObject, connection, endpoint);
   Message errorMessage;
   if (!message.parse(errorMessage))
     errorMessage.send();
   else
     handleValidMessage(message);
   @endcode
   *
   * The Request ids and methods are stored in an internal lookup table upon
   * sending. This is used to set the method of the incoming reply. If the
   * lookup fails, the message will be parsed properly, but the method attribute
   * will not be set.
   */
  bool parse();
  bool parse(Message &errorMessage_);
  /**@}*/

private:
  /**
   * @brief Validate the message type.
   * @param method String representation of the method calling this function.
   * @param validTypes Bitwise-or combination of allowed types.
   * @return True if the type is valid, false otherwise.
   *
   * A warning will be printed if the type is invalid.
   */
  bool checkType(const char *method_, MessageTypes validTypes) const;

  /**
   * @{
   * Helper functions for parse(). Validate and intepret @a json. @a
   * errorMessage is used for error handling.
   */
  bool interpretRequest(const QJsonObject &json, Message &errorMessage);
  void interpretNotification(const QJsonObject &json);
  void interpretResponse(const QJsonObject &json, const QString &method_);
  void interpretError(const QJsonObject &json, const QString &method_);
  /**@}*/

  /// Type of message
  MessageType m_type;

  /**
   * @{
   * Data storage.
   */
  QString m_method;
  MessageIdType m_id;
  QJsonValue m_params;
  QJsonValue m_result;
  int m_errorCode;
  QString m_errorMessage;
  QJsonValue m_errorData;
  QJsonObject m_rawJson;
  /**@}*/

  /// Connection from which the message originated
  Connection *m_connection;

  /// Used internally by Connection subclasses.
  EndpointIdType m_endpoint;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Message::MessageTypes)

} // namespace MoleQueue

#endif // MOLEQUEUE_MESSAGE_H
