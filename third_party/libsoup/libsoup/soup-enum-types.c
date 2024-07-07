/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
/* Generated data (by glib-mkenums) */

#include <config.h>

#define LIBSOUP_USE_UNSTABLE_REQUEST_API
#include <libsoup/soup.h>

#define C_ENUM(v) ((gint) v)
#define C_FLAGS(v) ((guint) v)

/* enumerations from "soup-address.h" */
GType
soup_address_family_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_ADDRESS_FAMILY_INVALID), "SOUP_ADDRESS_FAMILY_INVALID", "invalid" },
    { C_ENUM(SOUP_ADDRESS_FAMILY_IPV4), "SOUP_ADDRESS_FAMILY_IPV4", "ipv4" },
    { C_ENUM(SOUP_ADDRESS_FAMILY_IPV6), "SOUP_ADDRESS_FAMILY_IPV6", "ipv6" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupAddressFamily", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-cache.h" */
GType
soup_cacheability_get_type (void)
{
  static gsize id = 0;
  static const GFlagsValue values[] = {
    { C_FLAGS(SOUP_CACHE_CACHEABLE), "SOUP_CACHE_CACHEABLE", "cacheable" },
    { C_FLAGS(SOUP_CACHE_UNCACHEABLE), "SOUP_CACHE_UNCACHEABLE", "uncacheable" },
    { C_FLAGS(SOUP_CACHE_INVALIDATES), "SOUP_CACHE_INVALIDATES", "invalidates" },
    { C_FLAGS(SOUP_CACHE_VALIDATES), "SOUP_CACHE_VALIDATES", "validates" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_flags_register_static ("SoupCacheability", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_cache_response_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_CACHE_RESPONSE_FRESH), "SOUP_CACHE_RESPONSE_FRESH", "fresh" },
    { C_ENUM(SOUP_CACHE_RESPONSE_NEEDS_VALIDATION), "SOUP_CACHE_RESPONSE_NEEDS_VALIDATION", "needs-validation" },
    { C_ENUM(SOUP_CACHE_RESPONSE_STALE), "SOUP_CACHE_RESPONSE_STALE", "stale" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupCacheResponse", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_cache_type_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_CACHE_SINGLE_USER), "SOUP_CACHE_SINGLE_USER", "single-user" },
    { C_ENUM(SOUP_CACHE_SHARED), "SOUP_CACHE_SHARED", "shared" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupCacheType", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-cookie.h" */
GType
soup_same_site_policy_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_SAME_SITE_POLICY_NONE), "SOUP_SAME_SITE_POLICY_NONE", "none" },
    { C_ENUM(SOUP_SAME_SITE_POLICY_LAX), "SOUP_SAME_SITE_POLICY_LAX", "lax" },
    { C_ENUM(SOUP_SAME_SITE_POLICY_STRICT), "SOUP_SAME_SITE_POLICY_STRICT", "strict" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupSameSitePolicy", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-cookie-jar.h" */
GType
soup_cookie_jar_accept_policy_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_COOKIE_JAR_ACCEPT_ALWAYS), "SOUP_COOKIE_JAR_ACCEPT_ALWAYS", "always" },
    { C_ENUM(SOUP_COOKIE_JAR_ACCEPT_NEVER), "SOUP_COOKIE_JAR_ACCEPT_NEVER", "never" },
    { C_ENUM(SOUP_COOKIE_JAR_ACCEPT_NO_THIRD_PARTY), "SOUP_COOKIE_JAR_ACCEPT_NO_THIRD_PARTY", "no-third-party" },
    { C_ENUM(SOUP_COOKIE_JAR_ACCEPT_GRANDFATHERED_THIRD_PARTY), "SOUP_COOKIE_JAR_ACCEPT_GRANDFATHERED_THIRD_PARTY", "grandfathered-third-party" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupCookieJarAcceptPolicy", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-date.h" */
GType
soup_date_format_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_DATE_HTTP), "SOUP_DATE_HTTP", "http" },
    { C_ENUM(SOUP_DATE_COOKIE), "SOUP_DATE_COOKIE", "cookie" },
    { C_ENUM(SOUP_DATE_RFC2822), "SOUP_DATE_RFC2822", "rfc2822" },
    { C_ENUM(SOUP_DATE_ISO8601_COMPACT), "SOUP_DATE_ISO8601_COMPACT", "iso8601-compact" },
    { C_ENUM(SOUP_DATE_ISO8601_FULL), "SOUP_DATE_ISO8601_FULL", "iso8601-full" },
    { C_ENUM(SOUP_DATE_ISO8601), "SOUP_DATE_ISO8601", "iso8601" },
    { C_ENUM(SOUP_DATE_ISO8601_XMLRPC), "SOUP_DATE_ISO8601_XMLRPC", "iso8601-xmlrpc" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupDateFormat", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-logger.h" */
GType
soup_logger_log_level_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_LOGGER_LOG_NONE), "SOUP_LOGGER_LOG_NONE", "none" },
    { C_ENUM(SOUP_LOGGER_LOG_MINIMAL), "SOUP_LOGGER_LOG_MINIMAL", "minimal" },
    { C_ENUM(SOUP_LOGGER_LOG_HEADERS), "SOUP_LOGGER_LOG_HEADERS", "headers" },
    { C_ENUM(SOUP_LOGGER_LOG_BODY), "SOUP_LOGGER_LOG_BODY", "body" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupLoggerLogLevel", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-message.h" */
GType
soup_http_version_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_HTTP_1_0), "SOUP_HTTP_1_0", "http-1-0" },
    { C_ENUM(SOUP_HTTP_1_1), "SOUP_HTTP_1_1", "http-1-1" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupHTTPVersion", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_message_flags_get_type (void)
{
  static gsize id = 0;
  static const GFlagsValue values[] = {
    { C_FLAGS(SOUP_MESSAGE_NO_REDIRECT), "SOUP_MESSAGE_NO_REDIRECT", "no-redirect" },
    { C_FLAGS(SOUP_MESSAGE_CAN_REBUILD), "SOUP_MESSAGE_CAN_REBUILD", "can-rebuild" },
    { C_FLAGS(SOUP_MESSAGE_OVERWRITE_CHUNKS), "SOUP_MESSAGE_OVERWRITE_CHUNKS", "overwrite-chunks" },
    { C_FLAGS(SOUP_MESSAGE_CONTENT_DECODED), "SOUP_MESSAGE_CONTENT_DECODED", "content-decoded" },
    { C_FLAGS(SOUP_MESSAGE_CERTIFICATE_TRUSTED), "SOUP_MESSAGE_CERTIFICATE_TRUSTED", "certificate-trusted" },
    { C_FLAGS(SOUP_MESSAGE_NEW_CONNECTION), "SOUP_MESSAGE_NEW_CONNECTION", "new-connection" },
    { C_FLAGS(SOUP_MESSAGE_IDEMPOTENT), "SOUP_MESSAGE_IDEMPOTENT", "idempotent" },
    { C_FLAGS(SOUP_MESSAGE_IGNORE_CONNECTION_LIMITS), "SOUP_MESSAGE_IGNORE_CONNECTION_LIMITS", "ignore-connection-limits" },
    { C_FLAGS(SOUP_MESSAGE_DO_NOT_USE_AUTH_CACHE), "SOUP_MESSAGE_DO_NOT_USE_AUTH_CACHE", "do-not-use-auth-cache" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_flags_register_static ("SoupMessageFlags", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_message_priority_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_MESSAGE_PRIORITY_VERY_LOW), "SOUP_MESSAGE_PRIORITY_VERY_LOW", "very-low" },
    { C_ENUM(SOUP_MESSAGE_PRIORITY_LOW), "SOUP_MESSAGE_PRIORITY_LOW", "low" },
    { C_ENUM(SOUP_MESSAGE_PRIORITY_NORMAL), "SOUP_MESSAGE_PRIORITY_NORMAL", "normal" },
    { C_ENUM(SOUP_MESSAGE_PRIORITY_HIGH), "SOUP_MESSAGE_PRIORITY_HIGH", "high" },
    { C_ENUM(SOUP_MESSAGE_PRIORITY_VERY_HIGH), "SOUP_MESSAGE_PRIORITY_VERY_HIGH", "very-high" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupMessagePriority", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-message-body.h" */
GType
soup_memory_use_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_MEMORY_STATIC), "SOUP_MEMORY_STATIC", "static" },
    { C_ENUM(SOUP_MEMORY_TAKE), "SOUP_MEMORY_TAKE", "take" },
    { C_ENUM(SOUP_MEMORY_COPY), "SOUP_MEMORY_COPY", "copy" },
    { C_ENUM(SOUP_MEMORY_TEMPORARY), "SOUP_MEMORY_TEMPORARY", "temporary" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupMemoryUse", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-message-headers.h" */
GType
soup_message_headers_type_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_MESSAGE_HEADERS_REQUEST), "SOUP_MESSAGE_HEADERS_REQUEST", "request" },
    { C_ENUM(SOUP_MESSAGE_HEADERS_RESPONSE), "SOUP_MESSAGE_HEADERS_RESPONSE", "response" },
    { C_ENUM(SOUP_MESSAGE_HEADERS_MULTIPART), "SOUP_MESSAGE_HEADERS_MULTIPART", "multipart" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupMessageHeadersType", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_encoding_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_ENCODING_UNRECOGNIZED), "SOUP_ENCODING_UNRECOGNIZED", "unrecognized" },
    { C_ENUM(SOUP_ENCODING_NONE), "SOUP_ENCODING_NONE", "none" },
    { C_ENUM(SOUP_ENCODING_CONTENT_LENGTH), "SOUP_ENCODING_CONTENT_LENGTH", "content-length" },
    { C_ENUM(SOUP_ENCODING_EOF), "SOUP_ENCODING_EOF", "eof" },
    { C_ENUM(SOUP_ENCODING_CHUNKED), "SOUP_ENCODING_CHUNKED", "chunked" },
    { C_ENUM(SOUP_ENCODING_BYTERANGES), "SOUP_ENCODING_BYTERANGES", "byteranges" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupEncoding", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_expectation_get_type (void)
{
  static gsize id = 0;
  static const GFlagsValue values[] = {
    { C_FLAGS(SOUP_EXPECTATION_UNRECOGNIZED), "SOUP_EXPECTATION_UNRECOGNIZED", "unrecognized" },
    { C_FLAGS(SOUP_EXPECTATION_CONTINUE), "SOUP_EXPECTATION_CONTINUE", "continue" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_flags_register_static ("SoupExpectation", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-misc.h" */
GType
soup_connection_state_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_CONNECTION_NEW), "SOUP_CONNECTION_NEW", "new" },
    { C_ENUM(SOUP_CONNECTION_CONNECTING), "SOUP_CONNECTION_CONNECTING", "connecting" },
    { C_ENUM(SOUP_CONNECTION_IDLE), "SOUP_CONNECTION_IDLE", "idle" },
    { C_ENUM(SOUP_CONNECTION_IN_USE), "SOUP_CONNECTION_IN_USE", "in-use" },
    { C_ENUM(SOUP_CONNECTION_REMOTE_DISCONNECTED), "SOUP_CONNECTION_REMOTE_DISCONNECTED", "remote-disconnected" },
    { C_ENUM(SOUP_CONNECTION_DISCONNECTED), "SOUP_CONNECTION_DISCONNECTED", "disconnected" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupConnectionState", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-requester.h" */
GType
soup_requester_error_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_REQUESTER_ERROR_BAD_URI), "SOUP_REQUESTER_ERROR_BAD_URI", "bad-uri" },
    { C_ENUM(SOUP_REQUESTER_ERROR_UNSUPPORTED_URI_SCHEME), "SOUP_REQUESTER_ERROR_UNSUPPORTED_URI_SCHEME", "unsupported-uri-scheme" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupRequesterError", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-server.h" */
GType
soup_server_listen_options_get_type (void)
{
  static gsize id = 0;
  static const GFlagsValue values[] = {
    { C_FLAGS(SOUP_SERVER_LISTEN_HTTPS), "SOUP_SERVER_LISTEN_HTTPS", "https" },
    { C_FLAGS(SOUP_SERVER_LISTEN_IPV4_ONLY), "SOUP_SERVER_LISTEN_IPV4_ONLY", "ipv4-only" },
    { C_FLAGS(SOUP_SERVER_LISTEN_IPV6_ONLY), "SOUP_SERVER_LISTEN_IPV6_ONLY", "ipv6-only" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_flags_register_static ("SoupServerListenOptions", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-session.h" */
GType
soup_request_error_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_REQUEST_ERROR_BAD_URI), "SOUP_REQUEST_ERROR_BAD_URI", "bad-uri" },
    { C_ENUM(SOUP_REQUEST_ERROR_UNSUPPORTED_URI_SCHEME), "SOUP_REQUEST_ERROR_UNSUPPORTED_URI_SCHEME", "unsupported-uri-scheme" },
    { C_ENUM(SOUP_REQUEST_ERROR_PARSING), "SOUP_REQUEST_ERROR_PARSING", "parsing" },
    { C_ENUM(SOUP_REQUEST_ERROR_ENCODING), "SOUP_REQUEST_ERROR_ENCODING", "encoding" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupRequestError", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-socket.h" */
GType
soup_socket_io_status_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_SOCKET_OK), "SOUP_SOCKET_OK", "ok" },
    { C_ENUM(SOUP_SOCKET_WOULD_BLOCK), "SOUP_SOCKET_WOULD_BLOCK", "would-block" },
    { C_ENUM(SOUP_SOCKET_EOF), "SOUP_SOCKET_EOF", "eof" },
    { C_ENUM(SOUP_SOCKET_ERROR), "SOUP_SOCKET_ERROR", "error" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupSocketIOStatus", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-status.h" */
GType
soup_status_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_STATUS_NONE), "SOUP_STATUS_NONE", "none" },
    { C_ENUM(SOUP_STATUS_CANCELLED), "SOUP_STATUS_CANCELLED", "cancelled" },
    { C_ENUM(SOUP_STATUS_CANT_RESOLVE), "SOUP_STATUS_CANT_RESOLVE", "cant-resolve" },
    { C_ENUM(SOUP_STATUS_CANT_RESOLVE_PROXY), "SOUP_STATUS_CANT_RESOLVE_PROXY", "cant-resolve-proxy" },
    { C_ENUM(SOUP_STATUS_CANT_CONNECT), "SOUP_STATUS_CANT_CONNECT", "cant-connect" },
    { C_ENUM(SOUP_STATUS_CANT_CONNECT_PROXY), "SOUP_STATUS_CANT_CONNECT_PROXY", "cant-connect-proxy" },
    { C_ENUM(SOUP_STATUS_SSL_FAILED), "SOUP_STATUS_SSL_FAILED", "ssl-failed" },
    { C_ENUM(SOUP_STATUS_IO_ERROR), "SOUP_STATUS_IO_ERROR", "io-error" },
    { C_ENUM(SOUP_STATUS_MALFORMED), "SOUP_STATUS_MALFORMED", "malformed" },
    { C_ENUM(SOUP_STATUS_TRY_AGAIN), "SOUP_STATUS_TRY_AGAIN", "try-again" },
    { C_ENUM(SOUP_STATUS_TOO_MANY_REDIRECTS), "SOUP_STATUS_TOO_MANY_REDIRECTS", "too-many-redirects" },
    { C_ENUM(SOUP_STATUS_TLS_FAILED), "SOUP_STATUS_TLS_FAILED", "tls-failed" },
    { C_ENUM(SOUP_STATUS_CONTINUE), "SOUP_STATUS_CONTINUE", "continue" },
    { C_ENUM(SOUP_STATUS_SWITCHING_PROTOCOLS), "SOUP_STATUS_SWITCHING_PROTOCOLS", "switching-protocols" },
    { C_ENUM(SOUP_STATUS_PROCESSING), "SOUP_STATUS_PROCESSING", "processing" },
    { C_ENUM(SOUP_STATUS_OK), "SOUP_STATUS_OK", "ok" },
    { C_ENUM(SOUP_STATUS_CREATED), "SOUP_STATUS_CREATED", "created" },
    { C_ENUM(SOUP_STATUS_ACCEPTED), "SOUP_STATUS_ACCEPTED", "accepted" },
    { C_ENUM(SOUP_STATUS_NON_AUTHORITATIVE), "SOUP_STATUS_NON_AUTHORITATIVE", "non-authoritative" },
    { C_ENUM(SOUP_STATUS_NO_CONTENT), "SOUP_STATUS_NO_CONTENT", "no-content" },
    { C_ENUM(SOUP_STATUS_RESET_CONTENT), "SOUP_STATUS_RESET_CONTENT", "reset-content" },
    { C_ENUM(SOUP_STATUS_PARTIAL_CONTENT), "SOUP_STATUS_PARTIAL_CONTENT", "partial-content" },
    { C_ENUM(SOUP_STATUS_MULTI_STATUS), "SOUP_STATUS_MULTI_STATUS", "multi-status" },
    { C_ENUM(SOUP_STATUS_MULTIPLE_CHOICES), "SOUP_STATUS_MULTIPLE_CHOICES", "multiple-choices" },
    { C_ENUM(SOUP_STATUS_MOVED_PERMANENTLY), "SOUP_STATUS_MOVED_PERMANENTLY", "moved-permanently" },
    { C_ENUM(SOUP_STATUS_FOUND), "SOUP_STATUS_FOUND", "found" },
    { C_ENUM(SOUP_STATUS_MOVED_TEMPORARILY), "SOUP_STATUS_MOVED_TEMPORARILY", "moved-temporarily" },
    { C_ENUM(SOUP_STATUS_SEE_OTHER), "SOUP_STATUS_SEE_OTHER", "see-other" },
    { C_ENUM(SOUP_STATUS_NOT_MODIFIED), "SOUP_STATUS_NOT_MODIFIED", "not-modified" },
    { C_ENUM(SOUP_STATUS_USE_PROXY), "SOUP_STATUS_USE_PROXY", "use-proxy" },
    { C_ENUM(SOUP_STATUS_NOT_APPEARING_IN_THIS_PROTOCOL), "SOUP_STATUS_NOT_APPEARING_IN_THIS_PROTOCOL", "not-appearing-in-this-protocol" },
    { C_ENUM(SOUP_STATUS_TEMPORARY_REDIRECT), "SOUP_STATUS_TEMPORARY_REDIRECT", "temporary-redirect" },
    { C_ENUM(SOUP_STATUS_BAD_REQUEST), "SOUP_STATUS_BAD_REQUEST", "bad-request" },
    { C_ENUM(SOUP_STATUS_UNAUTHORIZED), "SOUP_STATUS_UNAUTHORIZED", "unauthorized" },
    { C_ENUM(SOUP_STATUS_PAYMENT_REQUIRED), "SOUP_STATUS_PAYMENT_REQUIRED", "payment-required" },
    { C_ENUM(SOUP_STATUS_FORBIDDEN), "SOUP_STATUS_FORBIDDEN", "forbidden" },
    { C_ENUM(SOUP_STATUS_NOT_FOUND), "SOUP_STATUS_NOT_FOUND", "not-found" },
    { C_ENUM(SOUP_STATUS_METHOD_NOT_ALLOWED), "SOUP_STATUS_METHOD_NOT_ALLOWED", "method-not-allowed" },
    { C_ENUM(SOUP_STATUS_NOT_ACCEPTABLE), "SOUP_STATUS_NOT_ACCEPTABLE", "not-acceptable" },
    { C_ENUM(SOUP_STATUS_PROXY_AUTHENTICATION_REQUIRED), "SOUP_STATUS_PROXY_AUTHENTICATION_REQUIRED", "proxy-authentication-required" },
    { C_ENUM(SOUP_STATUS_PROXY_UNAUTHORIZED), "SOUP_STATUS_PROXY_UNAUTHORIZED", "proxy-unauthorized" },
    { C_ENUM(SOUP_STATUS_REQUEST_TIMEOUT), "SOUP_STATUS_REQUEST_TIMEOUT", "request-timeout" },
    { C_ENUM(SOUP_STATUS_CONFLICT), "SOUP_STATUS_CONFLICT", "conflict" },
    { C_ENUM(SOUP_STATUS_GONE), "SOUP_STATUS_GONE", "gone" },
    { C_ENUM(SOUP_STATUS_LENGTH_REQUIRED), "SOUP_STATUS_LENGTH_REQUIRED", "length-required" },
    { C_ENUM(SOUP_STATUS_PRECONDITION_FAILED), "SOUP_STATUS_PRECONDITION_FAILED", "precondition-failed" },
    { C_ENUM(SOUP_STATUS_REQUEST_ENTITY_TOO_LARGE), "SOUP_STATUS_REQUEST_ENTITY_TOO_LARGE", "request-entity-too-large" },
    { C_ENUM(SOUP_STATUS_REQUEST_URI_TOO_LONG), "SOUP_STATUS_REQUEST_URI_TOO_LONG", "request-uri-too-long" },
    { C_ENUM(SOUP_STATUS_UNSUPPORTED_MEDIA_TYPE), "SOUP_STATUS_UNSUPPORTED_MEDIA_TYPE", "unsupported-media-type" },
    { C_ENUM(SOUP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE), "SOUP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE", "requested-range-not-satisfiable" },
    { C_ENUM(SOUP_STATUS_INVALID_RANGE), "SOUP_STATUS_INVALID_RANGE", "invalid-range" },
    { C_ENUM(SOUP_STATUS_EXPECTATION_FAILED), "SOUP_STATUS_EXPECTATION_FAILED", "expectation-failed" },
    { C_ENUM(SOUP_STATUS_UNPROCESSABLE_ENTITY), "SOUP_STATUS_UNPROCESSABLE_ENTITY", "unprocessable-entity" },
    { C_ENUM(SOUP_STATUS_LOCKED), "SOUP_STATUS_LOCKED", "locked" },
    { C_ENUM(SOUP_STATUS_FAILED_DEPENDENCY), "SOUP_STATUS_FAILED_DEPENDENCY", "failed-dependency" },
    { C_ENUM(SOUP_STATUS_INTERNAL_SERVER_ERROR), "SOUP_STATUS_INTERNAL_SERVER_ERROR", "internal-server-error" },
    { C_ENUM(SOUP_STATUS_NOT_IMPLEMENTED), "SOUP_STATUS_NOT_IMPLEMENTED", "not-implemented" },
    { C_ENUM(SOUP_STATUS_BAD_GATEWAY), "SOUP_STATUS_BAD_GATEWAY", "bad-gateway" },
    { C_ENUM(SOUP_STATUS_SERVICE_UNAVAILABLE), "SOUP_STATUS_SERVICE_UNAVAILABLE", "service-unavailable" },
    { C_ENUM(SOUP_STATUS_GATEWAY_TIMEOUT), "SOUP_STATUS_GATEWAY_TIMEOUT", "gateway-timeout" },
    { C_ENUM(SOUP_STATUS_HTTP_VERSION_NOT_SUPPORTED), "SOUP_STATUS_HTTP_VERSION_NOT_SUPPORTED", "http-version-not-supported" },
    { C_ENUM(SOUP_STATUS_INSUFFICIENT_STORAGE), "SOUP_STATUS_INSUFFICIENT_STORAGE", "insufficient-storage" },
    { C_ENUM(SOUP_STATUS_NOT_EXTENDED), "SOUP_STATUS_NOT_EXTENDED", "not-extended" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupStatus", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_known_status_code_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NONE), "SOUP_KNOWN_STATUS_CODE_NONE", "none" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CANCELLED), "SOUP_KNOWN_STATUS_CODE_CANCELLED", "cancelled" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CANT_RESOLVE), "SOUP_KNOWN_STATUS_CODE_CANT_RESOLVE", "cant-resolve" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CANT_RESOLVE_PROXY), "SOUP_KNOWN_STATUS_CODE_CANT_RESOLVE_PROXY", "cant-resolve-proxy" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CANT_CONNECT), "SOUP_KNOWN_STATUS_CODE_CANT_CONNECT", "cant-connect" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CANT_CONNECT_PROXY), "SOUP_KNOWN_STATUS_CODE_CANT_CONNECT_PROXY", "cant-connect-proxy" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_SSL_FAILED), "SOUP_KNOWN_STATUS_CODE_SSL_FAILED", "ssl-failed" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_IO_ERROR), "SOUP_KNOWN_STATUS_CODE_IO_ERROR", "io-error" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_MALFORMED), "SOUP_KNOWN_STATUS_CODE_MALFORMED", "malformed" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_TRY_AGAIN), "SOUP_KNOWN_STATUS_CODE_TRY_AGAIN", "try-again" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_TOO_MANY_REDIRECTS), "SOUP_KNOWN_STATUS_CODE_TOO_MANY_REDIRECTS", "too-many-redirects" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_TLS_FAILED), "SOUP_KNOWN_STATUS_CODE_TLS_FAILED", "tls-failed" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CONTINUE), "SOUP_KNOWN_STATUS_CODE_CONTINUE", "continue" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_SWITCHING_PROTOCOLS), "SOUP_KNOWN_STATUS_CODE_SWITCHING_PROTOCOLS", "switching-protocols" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_PROCESSING), "SOUP_KNOWN_STATUS_CODE_PROCESSING", "processing" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_OK), "SOUP_KNOWN_STATUS_CODE_OK", "ok" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CREATED), "SOUP_KNOWN_STATUS_CODE_CREATED", "created" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_ACCEPTED), "SOUP_KNOWN_STATUS_CODE_ACCEPTED", "accepted" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NON_AUTHORITATIVE), "SOUP_KNOWN_STATUS_CODE_NON_AUTHORITATIVE", "non-authoritative" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NO_CONTENT), "SOUP_KNOWN_STATUS_CODE_NO_CONTENT", "no-content" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_RESET_CONTENT), "SOUP_KNOWN_STATUS_CODE_RESET_CONTENT", "reset-content" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_PARTIAL_CONTENT), "SOUP_KNOWN_STATUS_CODE_PARTIAL_CONTENT", "partial-content" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_MULTI_STATUS), "SOUP_KNOWN_STATUS_CODE_MULTI_STATUS", "multi-status" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_MULTIPLE_CHOICES), "SOUP_KNOWN_STATUS_CODE_MULTIPLE_CHOICES", "multiple-choices" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_MOVED_PERMANENTLY), "SOUP_KNOWN_STATUS_CODE_MOVED_PERMANENTLY", "moved-permanently" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_FOUND), "SOUP_KNOWN_STATUS_CODE_FOUND", "found" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_MOVED_TEMPORARILY), "SOUP_KNOWN_STATUS_CODE_MOVED_TEMPORARILY", "moved-temporarily" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_SEE_OTHER), "SOUP_KNOWN_STATUS_CODE_SEE_OTHER", "see-other" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NOT_MODIFIED), "SOUP_KNOWN_STATUS_CODE_NOT_MODIFIED", "not-modified" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_USE_PROXY), "SOUP_KNOWN_STATUS_CODE_USE_PROXY", "use-proxy" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NOT_APPEARING_IN_THIS_PROTOCOL), "SOUP_KNOWN_STATUS_CODE_NOT_APPEARING_IN_THIS_PROTOCOL", "not-appearing-in-this-protocol" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_TEMPORARY_REDIRECT), "SOUP_KNOWN_STATUS_CODE_TEMPORARY_REDIRECT", "temporary-redirect" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_BAD_REQUEST), "SOUP_KNOWN_STATUS_CODE_BAD_REQUEST", "bad-request" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_UNAUTHORIZED), "SOUP_KNOWN_STATUS_CODE_UNAUTHORIZED", "unauthorized" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_PAYMENT_REQUIRED), "SOUP_KNOWN_STATUS_CODE_PAYMENT_REQUIRED", "payment-required" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_FORBIDDEN), "SOUP_KNOWN_STATUS_CODE_FORBIDDEN", "forbidden" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NOT_FOUND), "SOUP_KNOWN_STATUS_CODE_NOT_FOUND", "not-found" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_METHOD_NOT_ALLOWED), "SOUP_KNOWN_STATUS_CODE_METHOD_NOT_ALLOWED", "method-not-allowed" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NOT_ACCEPTABLE), "SOUP_KNOWN_STATUS_CODE_NOT_ACCEPTABLE", "not-acceptable" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_PROXY_AUTHENTICATION_REQUIRED), "SOUP_KNOWN_STATUS_CODE_PROXY_AUTHENTICATION_REQUIRED", "proxy-authentication-required" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_PROXY_UNAUTHORIZED), "SOUP_KNOWN_STATUS_CODE_PROXY_UNAUTHORIZED", "proxy-unauthorized" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_REQUEST_TIMEOUT), "SOUP_KNOWN_STATUS_CODE_REQUEST_TIMEOUT", "request-timeout" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_CONFLICT), "SOUP_KNOWN_STATUS_CODE_CONFLICT", "conflict" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_GONE), "SOUP_KNOWN_STATUS_CODE_GONE", "gone" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_LENGTH_REQUIRED), "SOUP_KNOWN_STATUS_CODE_LENGTH_REQUIRED", "length-required" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_PRECONDITION_FAILED), "SOUP_KNOWN_STATUS_CODE_PRECONDITION_FAILED", "precondition-failed" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_REQUEST_ENTITY_TOO_LARGE), "SOUP_KNOWN_STATUS_CODE_REQUEST_ENTITY_TOO_LARGE", "request-entity-too-large" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_REQUEST_URI_TOO_LONG), "SOUP_KNOWN_STATUS_CODE_REQUEST_URI_TOO_LONG", "request-uri-too-long" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_UNSUPPORTED_MEDIA_TYPE), "SOUP_KNOWN_STATUS_CODE_UNSUPPORTED_MEDIA_TYPE", "unsupported-media-type" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_REQUESTED_RANGE_NOT_SATISFIABLE), "SOUP_KNOWN_STATUS_CODE_REQUESTED_RANGE_NOT_SATISFIABLE", "requested-range-not-satisfiable" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_INVALID_RANGE), "SOUP_KNOWN_STATUS_CODE_INVALID_RANGE", "invalid-range" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_EXPECTATION_FAILED), "SOUP_KNOWN_STATUS_CODE_EXPECTATION_FAILED", "expectation-failed" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_UNPROCESSABLE_ENTITY), "SOUP_KNOWN_STATUS_CODE_UNPROCESSABLE_ENTITY", "unprocessable-entity" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_LOCKED), "SOUP_KNOWN_STATUS_CODE_LOCKED", "locked" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_FAILED_DEPENDENCY), "SOUP_KNOWN_STATUS_CODE_FAILED_DEPENDENCY", "failed-dependency" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_INTERNAL_SERVER_ERROR), "SOUP_KNOWN_STATUS_CODE_INTERNAL_SERVER_ERROR", "internal-server-error" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NOT_IMPLEMENTED), "SOUP_KNOWN_STATUS_CODE_NOT_IMPLEMENTED", "not-implemented" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_BAD_GATEWAY), "SOUP_KNOWN_STATUS_CODE_BAD_GATEWAY", "bad-gateway" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_SERVICE_UNAVAILABLE), "SOUP_KNOWN_STATUS_CODE_SERVICE_UNAVAILABLE", "service-unavailable" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_GATEWAY_TIMEOUT), "SOUP_KNOWN_STATUS_CODE_GATEWAY_TIMEOUT", "gateway-timeout" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED), "SOUP_KNOWN_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED", "http-version-not-supported" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_INSUFFICIENT_STORAGE), "SOUP_KNOWN_STATUS_CODE_INSUFFICIENT_STORAGE", "insufficient-storage" },
    { C_ENUM(SOUP_KNOWN_STATUS_CODE_NOT_EXTENDED), "SOUP_KNOWN_STATUS_CODE_NOT_EXTENDED", "not-extended" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupKnownStatusCode", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-tld.h" */
GType
soup_tld_error_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_TLD_ERROR_INVALID_HOSTNAME), "SOUP_TLD_ERROR_INVALID_HOSTNAME", "invalid-hostname" },
    { C_ENUM(SOUP_TLD_ERROR_IS_IP_ADDRESS), "SOUP_TLD_ERROR_IS_IP_ADDRESS", "is-ip-address" },
    { C_ENUM(SOUP_TLD_ERROR_NOT_ENOUGH_DOMAINS), "SOUP_TLD_ERROR_NOT_ENOUGH_DOMAINS", "not-enough-domains" },
    { C_ENUM(SOUP_TLD_ERROR_NO_BASE_DOMAIN), "SOUP_TLD_ERROR_NO_BASE_DOMAIN", "no-base-domain" },
    { C_ENUM(SOUP_TLD_ERROR_NO_PSL_DATA), "SOUP_TLD_ERROR_NO_PSL_DATA", "no-psl-data" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupTLDError", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-websocket.h" */
GType
soup_websocket_error_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_WEBSOCKET_ERROR_FAILED), "SOUP_WEBSOCKET_ERROR_FAILED", "failed" },
    { C_ENUM(SOUP_WEBSOCKET_ERROR_NOT_WEBSOCKET), "SOUP_WEBSOCKET_ERROR_NOT_WEBSOCKET", "not-websocket" },
    { C_ENUM(SOUP_WEBSOCKET_ERROR_BAD_HANDSHAKE), "SOUP_WEBSOCKET_ERROR_BAD_HANDSHAKE", "bad-handshake" },
    { C_ENUM(SOUP_WEBSOCKET_ERROR_BAD_ORIGIN), "SOUP_WEBSOCKET_ERROR_BAD_ORIGIN", "bad-origin" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupWebsocketError", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_websocket_connection_type_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_WEBSOCKET_CONNECTION_UNKNOWN), "SOUP_WEBSOCKET_CONNECTION_UNKNOWN", "unknown" },
    { C_ENUM(SOUP_WEBSOCKET_CONNECTION_CLIENT), "SOUP_WEBSOCKET_CONNECTION_CLIENT", "client" },
    { C_ENUM(SOUP_WEBSOCKET_CONNECTION_SERVER), "SOUP_WEBSOCKET_CONNECTION_SERVER", "server" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupWebsocketConnectionType", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_websocket_data_type_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_WEBSOCKET_DATA_TEXT), "SOUP_WEBSOCKET_DATA_TEXT", "text" },
    { C_ENUM(SOUP_WEBSOCKET_DATA_BINARY), "SOUP_WEBSOCKET_DATA_BINARY", "binary" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupWebsocketDataType", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_websocket_close_code_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_NORMAL), "SOUP_WEBSOCKET_CLOSE_NORMAL", "normal" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_GOING_AWAY), "SOUP_WEBSOCKET_CLOSE_GOING_AWAY", "going-away" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_PROTOCOL_ERROR), "SOUP_WEBSOCKET_CLOSE_PROTOCOL_ERROR", "protocol-error" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_UNSUPPORTED_DATA), "SOUP_WEBSOCKET_CLOSE_UNSUPPORTED_DATA", "unsupported-data" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_NO_STATUS), "SOUP_WEBSOCKET_CLOSE_NO_STATUS", "no-status" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_ABNORMAL), "SOUP_WEBSOCKET_CLOSE_ABNORMAL", "abnormal" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_BAD_DATA), "SOUP_WEBSOCKET_CLOSE_BAD_DATA", "bad-data" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_POLICY_VIOLATION), "SOUP_WEBSOCKET_CLOSE_POLICY_VIOLATION", "policy-violation" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_TOO_BIG), "SOUP_WEBSOCKET_CLOSE_TOO_BIG", "too-big" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_NO_EXTENSION), "SOUP_WEBSOCKET_CLOSE_NO_EXTENSION", "no-extension" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_SERVER_ERROR), "SOUP_WEBSOCKET_CLOSE_SERVER_ERROR", "server-error" },
    { C_ENUM(SOUP_WEBSOCKET_CLOSE_TLS_HANDSHAKE), "SOUP_WEBSOCKET_CLOSE_TLS_HANDSHAKE", "tls-handshake" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupWebsocketCloseCode", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_websocket_state_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_WEBSOCKET_STATE_OPEN), "SOUP_WEBSOCKET_STATE_OPEN", "open" },
    { C_ENUM(SOUP_WEBSOCKET_STATE_CLOSING), "SOUP_WEBSOCKET_STATE_CLOSING", "closing" },
    { C_ENUM(SOUP_WEBSOCKET_STATE_CLOSED), "SOUP_WEBSOCKET_STATE_CLOSED", "closed" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupWebsocketState", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

/* enumerations from "soup-xmlrpc.h" */
GType
soup_xmlrpc_error_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_XMLRPC_ERROR_ARGUMENTS), "SOUP_XMLRPC_ERROR_ARGUMENTS", "arguments" },
    { C_ENUM(SOUP_XMLRPC_ERROR_RETVAL), "SOUP_XMLRPC_ERROR_RETVAL", "retval" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupXMLRPCError", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}

GType
soup_xmlrpc_fault_get_type (void)
{
  static gsize id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SOUP_XMLRPC_FAULT_PARSE_ERROR_NOT_WELL_FORMED), "SOUP_XMLRPC_FAULT_PARSE_ERROR_NOT_WELL_FORMED", "parse-error-not-well-formed" },
    { C_ENUM(SOUP_XMLRPC_FAULT_PARSE_ERROR_UNSUPPORTED_ENCODING), "SOUP_XMLRPC_FAULT_PARSE_ERROR_UNSUPPORTED_ENCODING", "parse-error-unsupported-encoding" },
    { C_ENUM(SOUP_XMLRPC_FAULT_PARSE_ERROR_INVALID_CHARACTER_FOR_ENCODING), "SOUP_XMLRPC_FAULT_PARSE_ERROR_INVALID_CHARACTER_FOR_ENCODING", "parse-error-invalid-character-for-encoding" },
    { C_ENUM(SOUP_XMLRPC_FAULT_SERVER_ERROR_INVALID_XML_RPC), "SOUP_XMLRPC_FAULT_SERVER_ERROR_INVALID_XML_RPC", "server-error-invalid-xml-rpc" },
    { C_ENUM(SOUP_XMLRPC_FAULT_SERVER_ERROR_REQUESTED_METHOD_NOT_FOUND), "SOUP_XMLRPC_FAULT_SERVER_ERROR_REQUESTED_METHOD_NOT_FOUND", "server-error-requested-method-not-found" },
    { C_ENUM(SOUP_XMLRPC_FAULT_SERVER_ERROR_INVALID_METHOD_PARAMETERS), "SOUP_XMLRPC_FAULT_SERVER_ERROR_INVALID_METHOD_PARAMETERS", "server-error-invalid-method-parameters" },
    { C_ENUM(SOUP_XMLRPC_FAULT_SERVER_ERROR_INTERNAL_XML_RPC_ERROR), "SOUP_XMLRPC_FAULT_SERVER_ERROR_INTERNAL_XML_RPC_ERROR", "server-error-internal-xml-rpc-error" },
    { C_ENUM(SOUP_XMLRPC_FAULT_APPLICATION_ERROR), "SOUP_XMLRPC_FAULT_APPLICATION_ERROR", "application-error" },
    { C_ENUM(SOUP_XMLRPC_FAULT_SYSTEM_ERROR), "SOUP_XMLRPC_FAULT_SYSTEM_ERROR", "system-error" },
    { C_ENUM(SOUP_XMLRPC_FAULT_TRANSPORT_ERROR), "SOUP_XMLRPC_FAULT_TRANSPORT_ERROR", "transport-error" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&id)) {
    GType tmp = g_enum_register_static ("SoupXMLRPCFault", values);
    g_once_init_leave (&id, tmp);
  }

  return (GType) id;
}



/* Generated data ends here */

