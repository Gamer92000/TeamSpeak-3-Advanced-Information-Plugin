/*
 * TeamSpeak 3 demo plugin
 *
 * Copyright (c) 2008-2016 TeamSpeak Systems GmbH
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#define PLUGINS_EXPORTDLL __declspec(dllexport)
#else
#define PLUGINS_EXPORTDLL __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /* Required functions */
  PLUGINS_EXPORTDLL const char *ts3plugin_name();
  PLUGINS_EXPORTDLL const char *ts3plugin_version();
  PLUGINS_EXPORTDLL int ts3plugin_apiVersion();
  PLUGINS_EXPORTDLL const char *ts3plugin_author();
  PLUGINS_EXPORTDLL const char *ts3plugin_description();
  PLUGINS_EXPORTDLL void ts3plugin_setFunctionPointers(const struct TS3Functions funcs);
  PLUGINS_EXPORTDLL int ts3plugin_init();
  PLUGINS_EXPORTDLL void ts3plugin_shutdown();

  /* Optional functions */
  PLUGINS_EXPORTDLL int ts3plugin_offersConfigure();
  PLUGINS_EXPORTDLL void ts3plugin_configure(void *handle, void *qParentWidget);
  PLUGINS_EXPORTDLL const char *ts3plugin_infoTitle();
  PLUGINS_EXPORTDLL void ts3plugin_infoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char **data);
  PLUGINS_EXPORTDLL void ts3plugin_freeMemory(void *data);
  PLUGINS_EXPORTDLL void ts3plugin_registerPluginID(const char *id);
  PLUGINS_EXPORTDLL void ts3plugin_initMenus(struct PluginMenuItem ***menuItems, char **menuIcon);

  PLUGINS_EXPORTDLL void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID);

#ifdef __cplusplus
}
#endif

#endif