#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

#include <stdint.h>
#include <trcTypes.h>
#include <trcStreamPortConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_INTERNAL_BUFFER (TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER)

/**
 * @def TRC_STREAM_PORT_BUFFER_SIZE
 *
 * @brief The buffer size, aligned to base type.
 */
#define TRC_STREAM_PORT_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

typedef struct TraceStreamPortBuffer
{
#if (TRC_USE_INTERNAL_BUFFER)
	uint8_t buffer[(TRC_STREAM_PORT_BUFFER_SIZE)];
#else
	TraceUnsignedBaseType_t buffer[1];
#endif
} TraceStreamPortBuffer_t;

int32_t trace_io_read(void* data, uint32_t size, int32_t* bytes_read);

int32_t trace_io_write(void* data, uint32_t size, int32_t* bytes_written);

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer);

#define xTraceStreamPortAllocate(uiSize, ppvData) ((void)(uiSize), xTraceStaticBufferGet(ppvData))

#if (TRC_USE_INTERNAL_BUFFER == 1)
/* Push to internal buffer. It will call on xTraceStreamPortWriteData() periodically. */
#define xTraceStreamPortCommit xTraceInternalEventBufferPush
#else
/* Write directly */
#define xTraceStreamPortCommit xTraceStreamPortWriteData
#endif

/* Return value is unused in trcEventBuffer.c */
/*#define xTraceStreamPortWriteData(pvData, uiSize, piBytesWritten) (trace_io_write(pvData, uiSize, piBytesWritten) == 0 ? TRC_SUCCESS : TRC_FAIL)*/
#define xTraceStreamPortWriteData(pvData, uiSize, piBytesWritten) trace_io_write(pvData, uiSize, piBytesWritten)

#define xTraceStreamPortReadData(pvData, uiSize, piBytesRead) (trace_io_read(pvData, uiSize, piBytesRead) == 0 ? TRC_SUCCESS : TRC_FAIL)

traceResult xTraceStreamPortOnEnable(uint32_t uiStartOption);
traceResult xTraceStreamPortOnDisable(void);
traceResult xTraceStreamPortOnTraceBegin(void);
traceResult xTraceStreamPortOnTraceEnd(void);

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
