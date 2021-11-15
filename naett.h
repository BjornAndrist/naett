#ifndef LIBNAETT_H
#define LIBNAETT_H

#ifdef __cplusplus
extern "C" {
#endif

#if __ANDROID__
#include <jni.h>
typedef JavaVM* naettInitData;
#else
typedef void* naettInitData;
#endif

/**
 * @brief Global init method.
 * Call to initialize the library.
 */
void naettInit(naettInitData initThing);

typedef struct naettReq naettReq;
typedef struct naettRes naettRes;
typedef int (*naettReadFunc)(void* dest, int bufferSize, void* userData);
typedef int (*naettWriteFunc)(const void* source, int bytes, void* userData);

// Option to `naettRequest`
typedef struct naettOption naettOption;

// Sets request method. Defaults to "GET".
naettOption* naettMethod(const char* method);
// Adds a request header.
naettOption* naettHeader(const char* name, const char* value);
// Sets the request body. Ignored if a body reader is configured.
naettOption* naettBody(const char* body, int size);
// Sets a request body reader.
naettOption* naettBodyReader(naettReadFunc reader, void* userData);
// Sets a response body writer.
naettOption* naettBodyWriter(naettWriteFunc writer, void* userData);
// Sets connection timeout in milliseconds.
naettOption* naettTimeout(int milliSeconds);

/**
 * @brief Creates a new request to the specified url.
 * Use varargs options to configure the connection and following request.
 */
#define naettRequest(url, ...) naettRequest_va(url, countOptions(__VA_ARGS__), ##__VA_ARGS__)

/**
 * @brief Creates a new request to the specified url.
 * Uses an array of options rather that varargs. 
 */
naettReq* naettRequestWithOptions(const char* url, int numOptions, const naettOption** options);

/**
 * @brief Makes a request and returns a response object.
 * The actual request is processed asynchronously, use `naettComplete`
 * to check if the response is completed.
 * 
 * A request object can be reused multiple times to make requests, but
 * there can be only one active request using the same request object.
 */
naettRes* naettMake(naettReq* request);

/**
 * @brief Frees a previously allocated request object.
 * The request must not have any pending responses.
 */
void naettFree(naettReq* request);

/**
 * @brief Checks if a response is complete, with a result
 * or with an error.
 * Use `naettGetStatus` to get the status.
 */
int naettComplete(const naettRes* response);

enum naettStatus {
    naettConnectionError = -1,
    naettProtocolError = -2,
    naettReadError = -3,
    naettProcessing = 0,
};

/**
 * @brief Returns the status of a response.
 * Status codes > 0 are HTTP status codes as returned by the server.
 * Status codes < 0 are processing errors according to the `naettStatus`
 * enum values.
 */
int naettGetStatus(const naettRes* response);

/**
 * @brief Returns the response body.
 * The body returned by this method is always empty when a custom
 * body reader has been set up using the `naettBodyReader` option.
 */
const void* naettGetBody(naettRes* response, int* outSize);

/**
 * @brief Returns the HTTP header value for the specified header name.
 */
const char* naettGetHeader(naettRes* response, const char* name);

/**
 * @brief Closes a response object.
 */
void naettClose(naettRes* response);

// Varargs glue
naettReq* naettRequest_va(const char* url, int numOptions, ...);
#define countOptions(...) ((sizeof((void*[]){ __VA_ARGS__ }) / sizeof(void*)))

#ifdef __cplusplus
}
#endif

#endif  // LIBNAETT_H
