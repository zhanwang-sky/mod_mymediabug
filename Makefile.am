-include $(top_srcdir)/build/modmake.rulesam
AUTOMAKE_OPTIONS = foreign subdir-objects
MODNAME=mod_mymediabug

LIBS := $(if $(switch_builddir),$(switch_builddir)/libfreeswitch.la,)

mod_LTLIBRARIES = mod_mymediabug.la
mod_mymediabug_la_SOURCES = mod_mymediabug.c curl_http.c
mod_mymediabug_la_CFLAGS  = $(AM_CFLAGS) $(FREESWITCH_CFLAGS) $(FFMPEG_CFLAGS) $(LIBCURL_CFLAGS) $(CJSON_CFLAGS)
mod_mymediabug_la_LDFLAGS = -avoid-version -module -no-undefined -shared $(FREESWITCH_LIBS) $(FFMPEG_LIBS) $(LIBCURL_LIBS) $(CJSON_LIBS)
mod_mymediabug_la_LIBADD  = $(LIBS)
