#pragma once

#include "context.h"

NTSTATUS ReadReport(IN PQUEUE_CONTEXT      QueueContext,
                    IN WDFREQUEST          Request,
                    _Always_(OUT) BOOLEAN *CompleteRequest);

NTSTATUS WriteReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request);

NTSTATUS GetFeature(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request);

NTSTATUS SetFeature(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request);

NTSTATUS GetInputReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request);

NTSTATUS SetOutputReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request);

NTSTATUS GetString(IN WDFREQUEST Request);

NTSTATUS GetIndexedString(IN WDFREQUEST Request);

NTSTATUS GetStringId(IN WDFREQUEST Request, OUT ULONG *StringId, OUT ULONG *LanguageId);
