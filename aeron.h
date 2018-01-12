#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
	#define AERON_CLIENT_CGO_EXPORT __declspec(dllexport)
#else
	#define AERON_CLIENT_CGO_EXPORT 
#endif 

typedef int (*poll_handler_t)(char*, int);

AERON_CLIENT_CGO_EXPORT int aeron_initialize(char *aeron_dir);
AERON_CLIENT_CGO_EXPORT void aeron_destroy();

AERON_CLIENT_CGO_EXPORT int aeron_add_publication(char *channel, int stream_id);
AERON_CLIENT_CGO_EXPORT void aeron_remove_publication(int publication_idx);
AERON_CLIENT_CGO_EXPORT int aeron_get_streamId(int publication_idx);
AERON_CLIENT_CGO_EXPORT int64_t aeron_publish(int publication_idx, char* msg, int msg_len);
AERON_CLIENT_CGO_EXPORT int aeron_add_subscription(char *channel, int stream_id);
AERON_CLIENT_CGO_EXPORT int aeron_poll(int subscription_idx, poll_handler_t handler);

#ifdef __cplusplus
}
#endif
