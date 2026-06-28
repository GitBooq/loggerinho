#include "core/enricher_factory.h"
#include "core/log_record.h"

#include "enrichers/IEnricher.h"
#include "enrichers/base_enricher.h"
#include "enrichers/enricher_flags.h"
#include "enrichers/timestamp_enricher.h"

#include "formatters/IFormatter.h"
#include "formatters/plain_text_formatter.h"
#include "formatters/json_formatter.h"

#include "sinks/ILog_sink.h"
#include "sinks/console_sink.h"