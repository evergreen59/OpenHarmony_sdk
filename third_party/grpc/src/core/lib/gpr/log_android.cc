/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <grpc/support/port_platform.h>

#ifdef GPR_ANDROID

#include <hilog/log.h>
#include <grpc/support/log.h>
#include <grpc/support/time.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const unsigned int DEVELOPTOOLS_GPRC_TAG = 0xD003300;

static LogLevel severity_to_log_priority(gpr_log_severity severity) {
  switch (severity) {
    case GPR_LOG_SEVERITY_DEBUG:
      return LOG_DEBUG;
    case GPR_LOG_SEVERITY_INFO:
      return LOG_INFO;
    case GPR_LOG_SEVERITY_ERROR:
      return LOG_ERROR;
  }
  return LOG_LEVEL_MIN;
}

void gpr_log(const char* file, int line, gpr_log_severity severity,
             const char* format, ...) {
  /* Avoid message construction if gpr_log_message won't log */
  if (gpr_should_log(severity) == 0) {
    return;
  }
  char* message = NULL;
  va_list args;
  va_start(args, format);
  vasprintf(&message, format, args);
  va_end(args);
  gpr_log_message(file, line, severity, message);
  free(message);
}

void gpr_default_log(gpr_log_func_args* args) {
  const char* final_slash;
  const char* display_file;

  final_slash = strrchr(args->file, '/');
  if (final_slash == NULL)
    display_file = args->file;
  else
    display_file = final_slash + 1;

  HiLogPrint(LOG_CORE, severity_to_log_priority(args->severity), DEVELOPTOOLS_GPRC_TAG, "GRPC", "%s:%d] %s", display_file, args->line, args->message);

}

#endif /* GPR_ANDROID */
