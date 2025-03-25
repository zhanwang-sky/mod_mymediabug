//
//  mod_mymediabug.c
//  mod_mymediabug
//
//  Created by zhanwang-sky on 2025/3/25.
//

#include <switch.h>

#define MB_CONFIG_FILE "mymediabug.conf"

SWITCH_MODULE_LOAD_FUNCTION(mod_mymediabug_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_mymediabug_shutdown);
SWITCH_MODULE_DEFINITION(mod_mymediabug, mod_mymediabug_load, mod_mymediabug_shutdown, NULL);

typedef struct mymediabug_session {
  switch_core_session_t* session;
  switch_media_bug_t* bug;
} mymediabug_session_t;

static struct {
  char* foo;
  int bar;
} mb_globals;

static switch_xml_config_item_t mb_instructions[] = {
  SWITCH_CONFIG_ITEM_STRING_STRDUP("foo", CONFIG_REQUIRED, &mb_globals.foo, "", "", "foo"),
  SWITCH_CONFIG_ITEM("bar", SWITCH_CONFIG_INT, CONFIG_RELOADABLE, &mb_globals.bar, (void *) 0, NULL,
                     "", "[optional] bar"),
  SWITCH_CONFIG_ITEM_END()
};

static switch_bool_t
mymediabug_cb(switch_media_bug_t* bug, void* data, switch_abc_type_t type) {
  switch_core_session_t* session = switch_core_media_bug_get_session(bug);
  mymediabug_session_t* mbs = (mymediabug_session_t*) data;
  switch_codec_t* read_codec = NULL;
  int sample_rate = -1;
  switch_bool_t ret = SWITCH_TRUE;

  switch (type) {
    case SWITCH_ABC_TYPE_INIT:
      do {
        mbs->session = session;
        mbs->bug = bug;

        read_codec = switch_core_session_get_read_codec(session);
        if (!read_codec || !read_codec->implementation) {
          switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_WARNING,
                            "mymediabug_cb[init]: read codec not set\n");
          ret = SWITCH_FALSE;
          break;
        }

        sample_rate = read_codec->implementation->samples_per_second;
        if (sample_rate < 8000 || sample_rate > 48000) {
          switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_WARNING,
                            "mymediabug_cb[init]: unsupported sample_rate: %d\n", sample_rate);
          ret = SWITCH_FALSE;
          break;
        }

        switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_NOTICE,
                          "mymediabug_cb[init]: mymediabug_session started, sample_rate=%d\n",
                          sample_rate);

      } while (0);

      if (ret != SWITCH_TRUE) {
        // XXX TODO: do clean
      }

      break; // case SWITCH_ABC_TYPE_INIT

    case SWITCH_ABC_TYPE_WRITE_REPLACE:
      // XXX TODO
      break;

    case SWITCH_ABC_TYPE_READ_REPLACE:
      // XXX TODO
      break;

    case SWITCH_ABC_TYPE_CLOSE:
      // XXX TODO: do clean
      switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_NOTICE,
                        "mymediabug_cb[close]: mymediabug_session ended\n");
      break;

    default:
      break;
  }

  return ret;
}

// static void name(switch_core_session_t *session, const char *data)
SWITCH_STANDARD_APP(mymediabug_app_func) {
  switch_channel_t* channel = NULL;
  mymediabug_session_t* mbs = NULL;
  switch_media_bug_t* bug = NULL;

  switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_NOTICE,
                    "app_func: data={%s}\n", data);

  channel = switch_core_session_get_channel(session);
  if (!channel) {
    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_WARNING,
                      "app_func: fail to get media channel\n");
    return;
  }

  if (!(mbs = switch_core_session_alloc(session, sizeof(mymediabug_session_t)))) {
    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_WARNING,
                      "app_func: fail to alloc mymediabug_session\n");
    return;
  }

  if (switch_core_media_bug_add(session, "mymediabug", NULL, mymediabug_cb, mbs, 0,
                                SMBF_READ_STREAM | SMBF_WRITE_STREAM,
                                &bug) != SWITCH_STATUS_SUCCESS) {
    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_WARNING,
                      "app_func: fail to add media bug!\n");
    return;
  }

  if (switch_channel_set_private(channel, "__mymediabug_session__", mbs) != SWITCH_STATUS_SUCCESS) {
    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_WARNING,
                      "app_func: fail to set channel private\n");
    return;
  }

  switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_NOTICE,
                    "app_func: media bug added\n");
}

// switch_status_t mod_mymediabug_load(switch_loadable_module_interface_t **module_interface, switch_memory_pool_t *pool)
SWITCH_MODULE_LOAD_FUNCTION(mod_mymediabug_load) {
  switch_application_interface_t* app_interface = NULL;

  // load config
  if (switch_xml_config_parse_module_settings(MB_CONFIG_FILE, SWITCH_FALSE, mb_instructions) != SWITCH_STATUS_SUCCESS) {
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CRIT, "fail to parse config file '%s'\n", MB_CONFIG_FILE);
    return SWITCH_STATUS_FALSE;
  }

  // connect my internal structure to the blank pointer passed to me
  *module_interface = switch_loadable_module_create_module_interface(pool, modname);

  SWITCH_ADD_APP(app_interface,
                 "mymediabug", "do something with media data", "mymediabug - do something with media data",
                 mymediabug_app_func, "rtmp://balabala/foo/bar", SAF_MEDIA_TAP);

  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "%s loaded\n", modname);

  return SWITCH_STATUS_SUCCESS;
}

// switch_status_t mod_mymediabug_shutdown(void)
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_mymediabug_shutdown) {
  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "shutting down %s...\n", modname);
  // XXX TODO: cleanup
  return SWITCH_STATUS_SUCCESS;
}
