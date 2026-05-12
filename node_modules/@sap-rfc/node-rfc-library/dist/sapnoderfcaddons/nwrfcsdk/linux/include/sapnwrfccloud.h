#ifndef SAPNWRFCCLOUD_H
#define SAPNWRFCCLOUD_H

#include "sapnwrfc.h"
#ifdef __cplusplus
extern "C"
{
#endif

	/** \struct _RFC_CONNECTION_POOL_HANDLE
	* \ingroup connection
	*
	* Handle to an RFC connection pool containing client connections.
	* Returned by RfcCreateConnectionPool()
	*/
	typedef struct _RFC_CONNECTION_POOL_HANDLE { void* handle; } *RFC_CONNECTION_POOL_HANDLE;

	/** \enum _RFC_TRACING
	* \ingroup api
	*
	* An enumeration of individual events that can be traced set via %RfcSetTraceComponents()
	* The trace level is a bit vector composed of individual trace events (bits)
	* from this enumeration.
	* As a convenience this enumeration offers elements <c>Level1</c> through <c>Level4</c>
	* representing increasingly detailed trace levels that can be enhanced with
	* individual trace events using bitwise <c>OR</c>.
	*/
	typedef enum _RFC_TRACING
	{
		None = 0x0,							///< The convenience element for suppressing all trace output
		RemoteFunctionCall = 0x1,			///< Traces details about remote function calls and related callstacks
		Connections = 0x2,					///< Traces most methods related to connections and servers like opening connections or starting servers
		Metadata = 0x4,						///< Traces most methods related to metadata, creation of metadata or containers
		RfcData = 0x8,						///< Traces the hexdumps during each remote function call, and (de-)serialization
		ParameterData = 0x10,				///< Traces the container data sent and received during each remote function call
		RepositoryAction = 0x20,			///< Traces the changes in the repository such as adding or deleting metadata
		Transactions = 0x40,				///< Traces methods related to tRFC or bgRFC
		Network = 0x80,						///< Traces lower level network data, i. e. pings/pongs
		Runtime = 0x100,					///< Traces information about changes in the runtime and config behaviour
		SetterGetter = 0x200,				///< Traces setter and getter operations and related methods
		TableOperations = 0x400,			///< Traces table operations such as looping through rows or adding rows
		Conversions = 0x800,				///< Traces operations related to codepage conversions etc
		Locking = 0x1000,					///< Writes data to the trace files that shows when threads request, acquire, and release locks on objects
		Security = 0x2000,					///< Traces TLS/SNC related information and authorization or authentication events
		ThirdPartyLibrary = 0x4000,         ///< Activates traces in third party libraries such as EPP Passport library to redirect the trace to the RFC trace

		Level1 = Connections | Runtime,
		///< The convenience element for tracing changes in the runtime and superficial information. This corresponds to level 1 in %RfcSetTraceLevel().
		Level2 = Level1 | RemoteFunctionCall | Metadata | ParameterData | Transactions,
		///< The convenience element for tracing most relevant data. This corresponds to level 2 in %RfcSetTraceLevel().
		Level3 = Level2 | RfcData | Network | Security | RepositoryAction,
		///< The convenience element for tracing detailed information about RFC calls and network operations. This corresponds to level 3 in %RfcSetTraceLevel().
		Level4 = Level3 | SetterGetter | TableOperations | Conversions | Locking | ThirdPartyLibrary
		///< The convenience element for tracing everything. This corresponds to level 4 in %RfcSetTraceLevel().
	}RFC_TRACING;

	typedef void (SAP_API* RFC_EXTERNAL_TRACE_WRITE)(const SAP_UC* const text, SAP_ULLONG textLength);

	/**
	* \brief  Sets the global trace as detailed components as a bitmask of RFC_TRACING flags.
	 * \ingroup general
	 *
	 * \note The trace level is a bit vector composed of individual trace events (bits)
	* from the enumeration.
	* As a convenience this enumeration offers elements <c>Level1</c> through <c>Level4</c>
	 * representing increasingly detailed trace levels that can be enhanced with
	 * individual trace events using bitwise <c>OR</c>.
	 *
	 * \in traceBitmask Bit mask composed of individual trace events (bits) from RFC_TRACING.
	 * \out *errorInfo Detail information in case of an error.
	 * \return RFC_OK
	 *
	 */
	DECL_EXP RFC_RC SAP_API RfcSetTraceComponents(SAP_ULLONG traceBitmask, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Registers a callback function of type RFC_EXTERNAL_TRACE_WRITE to write the trace to different target.
	* \ingroup general
	 *
	 * This function replaces the internal trace writing with an external user function. Any time an entry is traced, the callback function
	 * is called. Upon registering the well known trace file header is written once. Failures in calling the function will be logged
	 * to dev file.
	 * The user/application has to handle rolling files, multithreading and/or locking.
	 *
	 * \in externalTraceWrite Function pointer to a callback function for tracing.
	 * \in bitOptions Bit mask setting specific options about the trace and format.
	 * \out *errorInfo Detail information in case of an error.
	 * \return RFC_OK
	 */
	DECL_EXP RFC_RC SAP_API RfcRegisterExternalTrace(RFC_EXTERNAL_TRACE_WRITE externalTraceWrite, SAP_ULLONG bitOptions, RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  Unregisters the callback function of type RFC_EXTERNAL_TRACE_WRITE.
	 * \ingroup general
	 *
	 * This function removes the registered callback function. Only after unregiserting a new callback function
	 * can be installed.
	 *
	 * \out *errorInfo Detail information in case of an error.
	 * \return RFC_OK
	 */
	DECL_EXP RFC_RC SAP_API RfcUnregisterExternalTrace(RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  A convenience function to write the well known trace file header with the already registered RFC_EXTERNAL_TRACE_WRITE function.
	 * \ingroup general
	 *
	 * This function should be called when rolling files.
	 *
	 * \out *errorInfo Detail information in case of an error.
	 * \return RFC_OK
	 */
	DECL_EXP RFC_RC SAP_API RfcExternalTraceWriteFileHeader(RFC_ERROR_INFO* errorInfo);

	/** \brief  Sets a RFC_CHANGING table parameter with name RUCKSACK to the connection that can be used to piggyback data when invoking a function module.
	* \ingroup connection
	*
	* Upon setting the RUCKSACK table it can be used as "normal" parameter on the connection similar to a parameter of a function module. The table will be transmitted in the RFC data stream
	* and can be used in the function module, if the session has already a registered RUCKSACK done via the static ABAP method cl_apcrfc_rucksack=>regrucksack( CHANGING rucksack = <rucksack_variable> ).
	* A RUCKSACK can also be sent back by calling cl_apcrfc_rucksack=>regrucksack( EXPORTING rucksack = <rucksack_variable> ) in the function module.
	* A <rucksack_variable> has to be a global variable and lives in the scope of the session/connection.
	* Since the parameter description allows hard-coded metadata and type descriptions, this function has to be used with the utmost care.
	* The RUCKSACK does only work with the column-based serialization and cannot be used as RFC_TABLES parameter. See the sapnwrfc.ini for more details on that connection parameter.
	* The parameter will be deleted, when the connection is closed or RfcRemoveSessionRucksack is called.
	*
	* \in rfcHandle Client connection over which to execute the function module.
	* \in *rucksack Parameter description of the table.
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcSetSessionRucksack(RFC_CONNECTION_HANDLE rfcHandle, const RFC_PARAMETER_DESC* const rucksack, RFC_ERROR_INFO* errorInfo);

	/** \brief  Returns a handle to session parameter RUCKSACK.
	* \ingroup connection
	*
	* If that field has not yet been accessed/filled previously, a new empty structure is created.
	* \warning The memory of the table will be released, when the connection is closed or RfcRemoveSessionRucksack is called.
	* So don't destroy the returned table handle, nor continue to use it, after the
	* connection is closed or the RUCKSACK was removed!
	*
	* \in rfcHandle Client connection on which the RUCKSACK was registered.
	* \out *tableHandle A handle to the sub-table.
	* \out *errorInfo Field does not exist, is of wrong type or out of memory.
	* \return RFC_RC
	*/

	DECL_EXP RFC_RC SAP_API RfcGetSessionRucksack(RFC_CONNECTION_HANDLE rfcHandle, RFC_TABLE_HANDLE* tableHandle, RFC_ERROR_INFO* errorInfo);

	/** \brief  Removes the session RUCKSACK parameter from the connection and deletes its content.
	* \ingroup connection
	*
	* \in rfcHandle Client connection on which the RUCKSACK was registered.
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcRemoveSessionRucksack(RFC_CONNECTION_HANDLE rfcHandle, RFC_ERROR_INFO* errorInfo);

	/** \brief  Stores a pointer to a custom user object on a given connection to retrieve at a later point.
	* \ingroup connection
	*
	* \in rfcHandle Connection on which pointer shall be stored.
	* \in *pData Pointer to the custom object.
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcSetCustomData(RFC_CONNECTION_HANDLE rfcHandle, void* pData, RFC_ERROR_INFO* errorInfo);

	/** \brief  Retrieves the stored pointer to a custom user object on a given connection.
	* \ingroup connection
	*
	* \in rfcHandle Connection on which pointer shall be stored.
	* \out **pData Pointer to the custom object.
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetCustomData(RFC_CONNECTION_HANDLE rfcHandle, void** pData, RFC_ERROR_INFO* errorInfo);

	/** \brief  Cleares the pointer set by %RfcSetCustomData() on a given connection.
	* \ingroup connection
	*
	* \in rfcHandle Connection on which pointer shall be stored.
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcRemoveCustomData(RFC_CONNECTION_HANDLE rfcHandle, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Creates an inMemory PSE file from a PEM.
	* \ingroup general
	*
	* The API creates an inMemory PSE by parsing the PEM. This particular PSE can then be referenced for client connections and servers with
	* the connection parameter TLS_CLIENT_INMEMORY_PSE or TLS_SERVER_INMEMORY_PSE, respectively, with the commonName. The PSEs are hold in memory and can be
	* edited or deleted during runtime. Creating inMemory PSEs should be treated with the highest care.
	* This solution offers an alternative when dealing with a file PSE is not possible or desired.
	* The distinguishedName has to be a valid distinguished name for the PSE, similar to when creating a file based PSE.
	* The PEM has to contain the private key, public certificate and trusted certificates.
	*
	* \note The crypto library has to be loaded before via API or sapnwrfc.ini.
	*
	* \in *distinguishedName The distinguished name of the owner of the PSE.
	* \in *pem The PEM as byte string.
	* \in pemLength The byte length of the PEM.
	* \in *passphrase Optional: Password/Passphrase for decryption of the private key. Null if no Passphrase is needed.
	* \out *errorInfo Detail information in case of an error.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcCreateInMemoryPSE(const SAP_UC* const distinguishedName, const SAP_RAW* const pem, unsigned pemLength, const SAP_UC* const passphrase, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Reloads an inMemory PSE file from a PEM.
	* \ingroup general
	*
	* The API empties an already existing inMemory PSE and reloads private key and trusted certificates by parsing the PEM.
	*
	* \note The crypto library has to be loaded before via API or sapnwrfc.ini.
	*
	* \in *distinguishedName The distinguished name of the owner of the PSE.
	* \in *pem The PEM as byte string.
	* \in pemLength The byte length of the PEM.
	* \in *passphrase Optional: Password/Passphrase for decryption of the private key. Null if no Passphrase is required.
	* \out *errorInfo Detail information in case of an error.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcReloadInMemoryPSE(const SAP_UC* const distinguishedName, const SAP_RAW* const pem, unsigned pemLength, const SAP_UC* const passphrase, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Deletes an inMemory PSE created by RfcCreateInMemoryPSE().
	* \ingroup general
	*
	* The API deletes the inMemory PSE that was created by RfcCreateInMemoryPSE(). The PSE will be no longer be available for client connections and servers
	* and freshly created connections can no longer be established by referencing this PSE. Deleting inMemory PSEs should be treated with the highest care.
	*
	* \note The crypto library has to be loaded before via API or sapnwrfc.ini.
	*
	* \in *distinguishedName The distinguished name of the owner of the PSE.
	* \out *errorInfo Detail information in case of an error.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcDeleteInMemoryPSE(const SAP_UC* const distinguishedName, RFC_ERROR_INFO* errorInfo);
	/**
	* \brief  Streaming like function to return the value of the specified field as string starting from position "offset".
	* \ingroup container
	*
	* The stringBuffer will be filled with a string representation of the field value starting from position "offset".
	* The result is not null-terminated. The buffer will be filled until the end of the buffer or the end of the field value is reached.
	* In the first case the function will still return RFC_OK and you can keep calling the function with updated offsets.
	* The result will not be null-terminated.
	*
	* The field specified by name needs to be of one of the following data types.

	* - RFCTYPE_CHAR
	* - RFCTYPE_STRING
	* - RFCTYPE_NUM
	* - RFCTYPE_DATE
	* - RFCTYPE_TIME
	*
	* \in dataHandle A data container (function handle, structure handle or table handle). If dataHandle
	* is a table handle, the function will read the field value of the current row.
	* \in *name The name of the field to read.
	* \out *charBuffer Pre-allocated buffer, which will receive the (converted) field value.
	* \in bufferLength Size of the buffer in SAP_UC.
	* \in offset Starting char offset from which the string is returned.
	* \out *charsWritten Always returns the number of characters written.
	* \out *errorInfo More information in case the field does not exist or a conversion fails.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetCharRange(DATA_CONTAINER_HANDLE dataHandle, SAP_UC const* name, RFC_CHAR* charBuffer, unsigned bufferLength, unsigned offset, unsigned* charsWritten, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Streaming like function to return the value of the specified field as string starting from position "offset".
	* \ingroup container
	*
	* This function works exactly like RfcGetCharRange(), the difference being that the field is
	* addressed by its index within the structure/table/function module. The first field has index 0,
	* last field has index n-1, the order of the fields is as defined in the ABAP DDIC.
	*
	*
	* \in dataHandle A data container (function handle, structure handle or table handle). If dataHandle
	* is a table handle, the function will read the field value of the current row.
	* \in index The index of the field to read.
	* \out *charBuffer Pre-allocated buffer, which will receive the (converted) field value.
	* \in bufferLength Size of the buffer in SAP_UC.
	* \in offset Starting char offset from which the string is returned.
	* \out *charsWritten Always returns the number of characters written.
	* \out *errorInfo More information in case the field does not exist or a conversion fails.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetCharRangeByIndex(DATA_CONTAINER_HANDLE dataHandle, unsigned index, RFC_CHAR* charBuffer, unsigned bufferLength, unsigned offset, unsigned* charsWritten, RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  Streaming like function to return the value of the specified field as byte array starting from position "offset".
	 * \ingroup container
	 *
	 * The buffer will be filled until the end of the buffer or the end of the field value is reached.
	 * In the first case the function will still return RFC_OK and you can keep calling the function with updated offsets.
	 *
	 * The field specified by name needs to be of one of the following data types:
	 * - RFCTYPE_XSTRING
	 * - RFCTYPE_BYTE
	 *
	 *
	 * \in dataHandle A data container (function handle, structure handle or table handle). If dataHandle
	 * is a table handle, the function will read the field value of the current row.
	 * \in *name The name of the field to read.
	 * \out *byteBuffer Pre-allocated buffer, which will receive the (converted) field value.
	 * \in bufferLength Size of the buffer in bytes.
	 * \in offset Starting byte offset from which the Xstring is returned.
	 * \out *bytesWritten Always returns the number of bytes written.
	 * \out *errorInfo More information in case the field does not exist or a conversion fails.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcGetByteRange(DATA_CONTAINER_HANDLE dataHandle, SAP_UC const* name, SAP_RAW* byteBuffer, unsigned bufferLength, unsigned offset, unsigned* bytesWritten, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Streaming like function to return the value of the specified field as byte array starting from position "offset".
	* \ingroup container
	*
	* This function works exactly like RfcGetByteRange(), the difference being that the field is
	* addressed by its index within the structure/table/function module. The first field has index 0,
	* last field has index n-1, the order of the fields is as defined in the ABAP DDIC.
	*
	*
	* \in dataHandle A data container (function handle, structure handle or table handle). If dataHandle
	* is a table handle, the function will read the field value of the current row.
	* \in index The index of the field to read.
	* \out *byteBuffer Pre-allocated buffer, which will receive the (converted) field value.
	* \in bufferLength Size of the buffer in bytes.
	* \in offset Starting byte offset from which the Xstring is returned.
	* \out *bytesWritten Always returns the number of bytes written.
	* \out *errorInfo More information in case the field does not exist or a conversion fails.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetByteRangeByIndex(DATA_CONTAINER_HANDLE dataHandle, unsigned index, SAP_RAW* byteBuffer, unsigned bufferLength, unsigned offset, unsigned* bytesWritten, RFC_ERROR_INFO* errorInfo);
	
	/**
	 * \brief  Returns the fields of the table serialized as JSON.
	 * \ingroup container
	 *
	 * This function creates a JSON by looping through the table and concatenating all fields in string representation.
	 * The buffer has to be allocated by the user. The function returns when the next serialized row would overflow the buffer or when the
	 * speficied amount of rows is serialized.
	 * The function will always a return a proper JSON and return the number of rows serialized. In case the "header" cannot be written,
	 * no characters will be written at all.
	 * The JSON is build as follows
	 * {
	 *   "header": { NW RFC SDK meta data },
	 *   "data": { actual data of the table }
	 * }
	 *
	 * \in tableHandle The table to serialize.
	 * \in *buffer The character buffer that will be filled with the JSON string.
	 * \in bufferLength Maximum length of the buffer provided.
	 * \in indexFrom Starting row of the table for the serialization. The table "counts" from 0.
	 * \in numberOfRows Maximum number of rows to serialization starting from indexFrom.
	 * \in bitOptions Bitmask for special options for the JSON.
	 * \out *outBufferLength Length of JSON string written into buffer.
	 * \out *rowsWritten Number of rows that could be serialized into buffer.
	 * \out *errorInfo More information in case the serialization fails or the buffer is too small.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcGetTableAsJson(RFC_TABLE_HANDLE tableHandle, SAP_UC* const buffer, SAP_ULLONG bufferLength, unsigned indexFrom, unsigned numberOfRows,
		unsigned bitOptions, SAP_ULLONG* outBufferLength, unsigned* rowsWritten, RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  Returns the fields of the structure serialized as JSON.
	 * \ingroup container
	 *
	 * This function creates a JSON by concatenating all fields in string representation.
	 * The buffer has to be allocated by the user. If the buffer is too small to contain the entire string, the functions returns an empty JSON.
	 * In case the "header" cannot be written, no characters will be written at all.
	 * The JSON is build as follows
	 * {
	 *   "header": { NW RFC SDK meta data },
	 *   "data": { actual data of the structure }
	 * }
	 *
	 * \in structureHandle The structure to serialize.
	 * \in *buffer The character buffer that will be filled with the JSON string.
	 * \in bufferLength Maximum length of the buffer provided.
	 * \in bitOptions Bitmask for special options for the JSON.
	 * \out *outBufferLength Length of JSON string written into buffer.
	 * \out *errorInfo More information in case the serialization fails or the buffer is too small.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcGetStructureAsJson(RFC_STRUCTURE_HANDLE structureHandle, SAP_UC* const buffer, SAP_ULLONG bufferLength,
		unsigned bitOptions, SAP_ULLONG* outBufferLength, RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  Returns the fully resolved metadata of the table serialized as JSON.
	 * \ingroup container
	 *
	 * This function creates a JSON by looping through the table descriptor and concatenating all metadata fields in string representation.
	 * The buffer has to be allocated by the user.
	 * The JSON is build as follows
	 * {
	 *   "header": { NW RFC SDK meta data },
	 *   "data": { actual metadata of the table }
	 * }
	 *
	 * \in tableHandle The table from which the metadata is to serialize.
	 * \in *buffer The character buffer that will be filled with the JSON string.
	 * \in bufferLength Maximum length of the buffer provided.
	 * \in bitOptions Bitmask for special options for the JSON.
	 * \out *outBufferLength Length of JSON string written into the buffer.
	 * \out *errorInfo More information in case the serialization fails or the buffer is too small.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcGetTableMetadataAsJson(RFC_TABLE_HANDLE tableHandle, SAP_UC* const buffer, SAP_ULLONG bufferLength,
		unsigned bitOptions, SAP_ULLONG* outBufferLength, RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  Returns the fully resolved metadata of the structure serialized as JSON.
	 * \ingroup container
	 *
	 * This function creates a JSON by looping through the structure descriptor and concatenating all metadata fields in string representation.
	 * The buffer has to be allocated by the user.
	 * The JSON is build as follows
	 * {
	 *   "header": { NW RFC SDK meta data },
	 *   "data": { actual metadata of the structure }
	 * }
	 *
	 * \in structureHandle The structure from which the metadata is to serialize.
	 * \in *buffer The character buffer that will be filled with the JSON string.
	 * \in bufferLength Maximum length of the buffer provided.
	 * \in bitOptions Bitmask for special options for the JSON.
	 * \out *outBufferLength Length of JSON string written into the buffer.
	 * \out *errorInfo More information in case the serialization fails or the buffer is too small.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcGetStructureMetadataAsJson(RFC_STRUCTURE_HANDLE structureHandle, SAP_UC* const buffer, SAP_ULLONG bufferLength,
		unsigned bitOptions, SAP_ULLONG* outBufferLength, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Sends a network ping and awaits a response from the partner.
	*
	* After the ping is sent, the API awaits a response up to a timeout.
	* If the partner does not respond in the given timeout, the API will return a error.
	* In case the connection is broken, this API can help to detect it.
	* If the connection is broken, close this connection with %RfcCloseConnection and re-open a new one.
	* 
	* Not applicable for CPIC based connections.
	*
	* \in rfcHandle connection handle to send the ping on.
	* \in timeoutInMs timeout in milliseconds.
	* \out *errorInfo More details in error case.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcIsPartnerReachable(RFC_CONNECTION_HANDLE rfcHandle, unsigned timeoutInMs, RFC_ERROR_INFO* errorInfo);


	/** \brief  Checks if an active RFC connection can use the column-based serialization. Dependent on the backend.
	 * \ingroup connection
	 *
	 * Specify the %COMPRESSION_TYPE value as format. The connection will check if the backend supports the CB serialization
	 * and sets the serialization format. For future calls on this connection the  column-based serialization will be used.
	 *
	 * \in rfcHandle Connection to enable the CB serialization for
	 * \out *format Any of "WAN", "LAN" or "OFF"
	 * \out *errorInfo Error details.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcEnableColumnBasedSerialization(RFC_CONNECTION_HANDLE rfcHandle, const SAP_UC* const format, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Sets the global idle time interval of a socket connection via the SCC in seconds after which
	* a keep alive Websocket ping packet is sent.
	* \ingroup general
	*
	* The default value is 300; valid values are 0 [off] and a range from 10 [ten seconds] to 86400 [one day].
	*
	* \in pingInterval Interval in seconds.
	* \out *errorInfo Detail information in case of an error.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcSetSccSocketPingInterval(unsigned pingInterval, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Sets the global timeout for SCC socket keep alive ping reply packet in seconds.
	* \ingroup general
	*
	* If no such so-called pong packet is received from the communication partner, i.e. the SCC, as a reply to a previously sent
	* keep alive ping packet within this timeout period, the connection is considered as broken and will be closed.
	* The default value is 60; valid values are 0 [off] and a range from 10 [ten seconds] to 3600 [one hour].
	* The timeout value should be set lower than the ping interval, so only one ping is active at a time.
	*
	* \in pongTimeout Timeout in seconds.
	* \out *errorInfo Detail information in case of an error.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcSetSccSocketPongTimeout(unsigned pongTimeout, RFC_ERROR_INFO* errorInfo);



	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations. 
	*
	* See %RfcGetCharsByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetCharsByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_CHAR* charBuffer, unsigned bufferLength, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetNumByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetNumByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_NUM* charBuffer, unsigned bufferLength, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetDateByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetDateByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_DATE emptyDate, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetTimeByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetTimeByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_TIME emptyTime, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetStringByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetStringByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, SAP_UC* stringBuffer, unsigned bufferLength, unsigned* stringLength, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetBytesByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetBytesByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, SAP_RAW* byteBuffer, unsigned bufferLength, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetXStringByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetXStringByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, SAP_RAW* byteBuffer, unsigned bufferLength, unsigned* xstringLength, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetInt1ByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetInt1ByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_INT1* value, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetInt2ByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetInt2ByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_INT2* value, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetIntByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetIntByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_INT* value, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetInt8ByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetInt8ByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_INT8* value, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetFloatByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetFloatByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_FLOAT* value, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetDecF16ByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetDecF16ByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_DECF16* value, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetDecF34ByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetDecF34ByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_DECF34* value, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetTableByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetTableByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_TABLE_HANDLE* subTableHandle, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetStructureByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetStructureByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, RFC_STRUCTURE_HANDLE* structHandle, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Convienence function to directly access a field in a table without the need of table operations.
	*
	* See %RfcGetStringLengthByIndex for more details.
	*/
	DECL_EXP RFC_RC SAP_API RfcGetStringLengthByIndexAndRow(RFC_TABLE_HANDLE tableHandle, unsigned index, unsigned row, unsigned* stringLength, RFC_ERROR_INFO* errorInfo);



	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_INT1)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, RFC_INT1 value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_INT1 for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a 1byte integer to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetInt1ByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_INT1 to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetInt1ColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_INT1 callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);



	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_INT2)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, RFC_INT2 value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_INT2 for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a 2byte integer to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetInt2ByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_INT2 to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetInt2ColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_INT2 callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);




	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_INT4)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, RFC_INT value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_INT4 for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a 4byte integer to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetIntByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_INT4 to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetInt4ColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_INT4 callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);


	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_INT8)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, RFC_INT8 value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_INT8 for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a 8byte integer to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	* 
	* See %RfcGetInt8ByIndex for applicable column types. 
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_INT8 to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetInt8ColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_INT8 callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);



	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_DOUBLE)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, RFC_FLOAT value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_DOUBLE for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a double to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetFloatByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_DOUBLE to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetDoubleColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_DOUBLE callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);


	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_DEC16)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, RFC_DECF16 value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_DEC16 for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a decfloat16 to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetDecF16ByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_DEC16 to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetDec16ColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_DEC16 callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);



	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_DEC34)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, RFC_DECF34 value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_DEC34 for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a decfloat34 to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetDec34ByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_DEC34 to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetDec34ColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_DEC34 callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);


	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_CHAR_ARRAY)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, const SAP_UC* const value, unsigned valueLengthInChars, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_CHAR_ARRAY for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a char array to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetCharsByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_CHAR_ARRAY to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetStringColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_CHAR_ARRAY callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);

	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_DATE)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, const SAP_UC* const value, unsigned valueLengthInChars, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_DATE for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a date value to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetDateByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_DATE to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetDateColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_DATE callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);

	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_TIME)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, const SAP_UC* const value, unsigned valueLengthInChars, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_TIME for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a time value to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetTimeByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_TIME to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetTimeColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_TIME callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);

	typedef RFC_RC(SAP_API* RFC_CALLBACK_CONVERT_BYTE)(RFC_TABLE_HANDLE tableHandle, RFCTYPE type, void* customPtr, const SAP_RAW* const value, unsigned valueLengthInBytes, SAP_RAW* externalBuffer, unsigned* currentExternalBufferPosInBytes, unsigned externalBufferSizeInBytes, unsigned currentRow, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Processes a column of a table and calls a user function of type RFC_CALLBACK_CONVERT_TIME for each row.
	*
	* The function loops over a part of a column and passes the value of each row as a time value to the user function for each value.
	* This can be used to serialize the column into an external buffer without the need of table operations.
	* For convenience a pointer to a custom object can be passed to used it in the callback.
	* The function returns prematurely on the first error encountered.
	*
	* See %RfcGetTimeByIndex for applicable column types.
	*
	* \in tableHandle table handle to get the column from
	* \in index index of the column
	* \in *externalBuffer buffer to copy to the data to
	* \in *externalBufferPositionInBytes current buffer position
	* \in externalBufferSizeInBytes size of the external buffer
	* \in callback function pointer of type RFC_CALLBACK_CONVERT_TIME to the callback implementation, which processes each row
	* \in *customPtr pointer to a custom object which shall be used in the callback
	* \in fromRow starting row from where the column shall be processed
	* \in numberOfRows number of rows to process
	* \out *rowsFetched number of rows the callback was called successfully
	* \out *errorInfo More error details in case something goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetByteColumnByIndex(RFC_TABLE_HANDLE tableHandle, unsigned index, SAP_RAW* externalBuffer, unsigned* externalBufferPositionInBytes, unsigned externalBufferSizeInBytes, RFC_CALLBACK_CONVERT_BYTE callback, void* customPtr, unsigned fromRow, unsigned numberOfRows, unsigned* rowsFetched, RFC_ERROR_INFO* errorInfo);


	/**
	* \brief  Opens an RFC client connection pool for invoking ABAP function modules in an ABAP backend.
	* \ingroup connection
	*
	* The connections will be stateless. The pool has a capacity for keeping idle connections, if set, which
	* shall be reused. If a new connection is required, the pool automatically opens a new connection.
	* Setting POOL_PEAK_LIMIT to zero results in an unlimited peak limit.
	*
	* \in *connectionParams An array of RFC_CONNECTION_PARAMETERs with the names as described above
	* and the values as necessary in your landscape.
	* \in paramCount Number of parameters in the above array.
	* \out *errorInfo Returns more error details, if the connect attempt fails.
	* \return A handle to an RFC client connection pool.
	*/
	DECL_EXP RFC_CONNECTION_POOL_HANDLE SAP_API RfcCreateConnectionPool(RFC_CONNECTION_PARAMETER const* connectionParams, unsigned paramCount, RFC_ERROR_INFO* errorInfo);

	/** \brief  Closes all connections in an RFC client connection pool and deletes the pool handle.
	* \ingroup connection
	*
	* This function cannot be called when other threads are still invoking function modules on the pool.
	*
	* \in poolHandle Connection pool handle to be closed
	* \out *errorInfo Error details in case closing the connections fails. (Can usually be ignored...)
	* \return RFC_RC or RFC_ILLEGAL_STATE
	*/
	DECL_EXP RFC_RC SAP_API RfcDestroyConnectionPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_ERROR_INFO* errorInfo);

	/** \brief  Gives the current count of idle and busy connections the pool has.
	* \ingroup connection
	*
	* \in poolHandle Connection pool handle to be closed
	* \out *idleCount Number of idle connections
	* \out *busyCount Number of busy connections
	* \out *errorInfo Error details in case closing the connections fails. (Can usually be ignored...)
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetConnectionCountFromPool(RFC_CONNECTION_POOL_HANDLE poolHandle, unsigned* idleCount, unsigned* busyCount, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Executes a function module in the backend system on the given pool.
	* \ingroup connection
	*
	* See %RfcInvoke().
	* The pool will use an idle connection - if POOL_CAPACITY is set - or create a new one to execute the function module. An idle connection can be checked with a ping by
	* setting POOL_CHECK_CONNECTION. If there are no free connections, the pool waits for POOL_MAX_GET_CLIENT_TIME milliseconds
	* to get a free connection. If the timeout expires, RFC_TIMEOUT is returned.
	*
	* \in poolHandle Connection pool on which the invoke takes place.
	* \inout funcHandle Data container containing the input data for the function module.
	* %RfcInvoke() will write the FM's output data into this container.
	* \out *errorInfo like RfcInvoke() or RFC_TIMEOUT.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcInvokeViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_FUNCTION_HANDLE funcHandle, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Pool API for RfcGetFunctionDesc().
	* \ingroup repository
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in *funcName Name of the function module to look up.
	* \out *errorInfo More error details in case something goes wrong.
	* \return The metadata description of the given function module.
	*/
	DECL_EXP RFC_FUNCTION_DESC_HANDLE SAP_API RfcGetFunctionDescViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, SAP_UC const* funcName, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Pool API for RfcGetTypeDesc().
	* \ingroup repository
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in *typeName Name of a DDIC structure or table.
	* \out *errorInfo More error details in case something goes wrong.
	* \return The metadata description of the given structure/table.
	*/
	DECL_EXP RFC_TYPE_DESC_HANDLE SAP_API RfcGetTypeDescViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, SAP_UC const* typeName, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Attaches a throughput object to a connection pool.
	* \ingroup throughput
	*
	* The throughput object will monitor all currently avaible connections - busy or idle - and all connections
	* that will be opened by pool.
	*
	* \in poolHandle A handle to the connection pool.
	* \in throughput A handle to the throughput object.
	* \out *errorInfo More details in error case.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcSetThroughputOnConnectionPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_THROUGHPUT_HANDLE throughput, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Returns the currently attached throughput object from a connection pool, if any.
	* \ingroup throughput
	*
	* Returns null if none is attached.
	*
	* \in poolHandle A handle to the connection pool.
	* \out *errorInfo More details in error case.
	* \return A handle to a throughput object.
	*/
	DECL_EXP RFC_THROUGHPUT_HANDLE SAP_API RfcGetThroughputFromConnectionPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Removes the throughput object from a connection pool. None of connections will be monitored any longer.
	* \ingroup throughput
	*
	* \in poolHandle A handle to the connection pool.
	* \out *errorInfo More details in error case.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcRemoveThroughputFromConnectionPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Sets the global pool expiration check period in milliseconds.
	*
	* Sets the period for the background checker thread to check idle connections in created pools for expiration.
	* Values below 10000 are prohibited.
	* The default value is 60000 milliseconds.
	*
	* The timeout can also be set via POOL_EXPIRATION_CHECK_PERIOD in the DEFAULT section of the sapnwrfc.ini file.
	*
	* \in checkPeriod gloabl logon timeout in seconds.
	* \out *errorInfo More details in error case.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcSetGlobalPoolExpirationCheckPeriod(unsigned checkPeriod, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Pool API for RfcGetUnitID().
	* \ingroup bgrfc
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \out uuid Preallocated buffer that will receive the 32 digits of the unit ID and a terminating zero.
	* \out *errorInfo Not much that can go wrong here...
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetUnitIDViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_UNITID uuid, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Pool API for RfcCreateUnit().
	* \ingroup bgrfc
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in uuid A 32 digit unique identifier of the unit.
	* \in *queueNames[] A list of bgRFC queues in the backend or NULL.
	* \in queueNameCount The length of the queueNames list.
	* \in *unitAttr A set of attributes that specify the behaviour of the unit in the backend.
	* \out *identifier This identifier can be used in later calls to RfcConfirmUnit() and RfcGetUnitState().
	* \out *errorInfo More details in case something goes wrong.
	* \return RFC_OK or RFC_MEMORY_INSUFFICIENT
	*/
	DECL_EXP RFC_UNIT_HANDLE SAP_API RfcCreateUnitViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_UNITID uuid, SAP_UC const* queueNames[], unsigned queueNameCount, const RFC_UNIT_ATTRIBUTES* unitAttr, RFC_UNIT_IDENTIFIER* identifier, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Pool API for RfcInvokeInUnit().
	* \ingroup bgrfc
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in unitHandle A valid (unsubmitted) bgRFC unit, which shall be sent into the backend system.
	* \in funcHandle A function module, whose payload (IMPORTING/CHANGING/TABLES) shall be added to the unit.
	* \out *errorInfo More details in case something goes wrong.
	* \return RFC_OK, RFC_INVALID_HANDLE or RFC_MEMORY_INSUFFICIENT
	*/
	DECL_EXP RFC_RC SAP_API RfcInvokeInUnitViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_UNIT_HANDLE unitHandle, RFC_FUNCTION_HANDLE funcHandle, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Pool API for RfcSubmitUnit().
	* \ingroup bgrfc
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in unitHandle A valid (unsubmitted) bgRFC unit, which shall be sent into the backend system.
	* \out *errorInfo More details in case something goes wrong.
	* \return RFC_OK, RFC_INVALID_HANDLE, RFC_MEMORY_INSUFFICIENT, RFC_COMMUNICATION_FAILURE
	*/
	DECL_EXP RFC_RC SAP_API RfcSubmitUnitViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_UNIT_HANDLE unitHandle, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Pool API for RfcConfirmUnit().
	* \ingroup bgrfc
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in *identifier The correct combination of UID and type of the unit. Best use the output you get from RfcCreateUnit().
	* \out *errorInfo More details in case something goes wrong.
	* \return RFC_OK, RFC_INVALID_HANDLE, RFC_COMMUNICATION_FAILURE
	*/
	DECL_EXP RFC_RC SAP_API RfcConfirmUnitViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_UNIT_IDENTIFIER* identifier, RFC_ERROR_INFO* errorInfo);
	
	/**
	* \brief  Pool API for RfcGetUnitState().
	* \ingroup bgrfc
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in *identifier The correct combination of UID and type of the unit. Best use the output you get from RfcCreateUnit().
	* \out *state The state of the unit. See RFC_UNIT_STATE for more details.
	* \out *errorInfo Not much that can go wrong here...
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetUnitStateViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_UNIT_IDENTIFIER* identifier, RFC_UNIT_STATE* state, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Pool API for RfcGetTransactionID().
	* \ingroup transaction
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \out tid Will be filled with the transaction ID.
	* \out *errorInfo Error information in case there is a problem with the connection.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcGetTransactionIDViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_TID tid, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Pool API for RfcCreateTransaction().
	* \ingroup transaction
	*
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in tid A unique 24 character ID.
	* \in *queueName For tRFC set this to NULL, for qRFC specify the name of a qRFC inbound queue in the backend.
	* \out *errorInfo Error information in case there is a problem with the connection.
	* \return A data container that can be filled with several function modules.
	*/
	DECL_EXP RFC_TRANSACTION_HANDLE SAP_API RfcCreateTransactionViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_TID tid, SAP_UC const* queueName, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Pool API for RfcInvokeInTransaction().
	* \ingroup transaction
	* 
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in tHandle A transaction handle created via RfcCreateTransaction().
	* \in funcHandle An RFC_FUNCTION_HANDLE, whose IMPORTING, CHANGING and TABLES parameters have been filled.
	* \out *errorInfo Actually there is nothing that can go wrong here except for invalid handles and out of memory.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcInvokeInTransactionViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_TRANSACTION_HANDLE tHandle, RFC_FUNCTION_HANDLE funcHandle, RFC_ERROR_INFO* errorInfo);

    /**
	* \brief  Pool API for RfcSubmitTransaction().
	* \ingroup transaction
	* 
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in tHandle A transaction handle filled with one or several function modules.
	* \out *errorInfo Additional error information in case anything goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcSubmitTransactionViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_TRANSACTION_HANDLE tHandle, RFC_ERROR_INFO* errorInfo);

	/**
	* \brief  Pool API for RfcConfirmTransactionID().
	* \ingroup transaction
	* 
	* The function can return RFC_TIMEOUT, if the pool cannot get a free connection within POOL_MAX_GET_CLIENT_TIME.
	*
	* \in poolHandle Client pool handle, that manages the connections
	* \in tid A unique 24 character ID.
	* \out *errorInfo Additional error information in case anything goes wrong.
	* \return RFC_RC
	*/
	DECL_EXP RFC_RC SAP_API RfcConfirmTransactionIDViaPool(RFC_CONNECTION_POOL_HANDLE poolHandle, RFC_TID tid, RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  Installs a callback function of type RFC_SERVER_FUNCTION, which will be invoked when a request for
	 * the function module corresponding to funcDescHandle is received from the ABAP system associated to the specific server.
	 * \ingroup installer
	 *
	 * Registered functions for a specific server has precedence of globally or systemID dependent registered functions.
	 *
	 * \in serverHandle Specific server that shall install the function.
	 * \in funcDescHandle A function description giving the name of the function module and its parameters.
	 * \in serverFunction Pointer to a C function of type RFC_SERVER_FUNCTION.
	 * \out *errorInfo More details in case something goes wrong.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcInstallServerSpecificFunction(RFC_SERVER_HANDLE serverHandle, RFC_FUNCTION_DESC_HANDLE funcDescHandle, RFC_SERVER_FUNCTION serverFunction, RFC_ERROR_INFO* errorInfo);

	/**
	 * \brief  Removes the RFC_SERVER_FUNCTION callback for a specific funcDescHandle that is installed on a specific server.
	 * \ingroup installer
	 *
	 *
	 * \in serverHandle Specific server that shall uninstall the function.
	 * \in funcDescHandle A function description giving the name of the function module and its parameters.
	 * \out *errorInfo More details in case something goes wrong.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcUninstallServerSpecificFunction(RFC_SERVER_HANDLE serverHandle, RFC_FUNCTION_DESC_HANDLE funcDescHandle, RFC_ERROR_INFO* errorInfo);


	/**
	 * \brief  Returns the ABAP repository description of a field, if any.
	 * \ingroup structure
	 *
	 *
	 * \in fieldDescr Field description struct.
	 * \out extendedDescription ABAP repository description.
	 * \out *errorInfo More details in case something goes wrong.
	 * \return RFC_RC
	 */
	DECL_EXP RFC_RC SAP_API RfcGetExtendedFieldDescription(const RFC_FIELD_DESC* fieldDescr, const SAP_UC** extendedDescription, RFC_ERROR_INFO* errorInfo);

#ifdef __cplusplus
}
#endif /*cplusplus*/

#endif // !SAPNWRFCCLOUD_H
