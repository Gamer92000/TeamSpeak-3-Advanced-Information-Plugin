#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#pragma warning(disable : 4100) /* Disable Unreferenced parameter warning */
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <QtCore/QDateTime>

#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "ts3_functions.h"

#include "plugin.h"
#include "definitions.hpp"
#include "helper.h"
#include "config.h"
#include "ui_config.h"
#include "sharedHeader.h"

static struct TS3Functions ts3Functions;
static char *pluginID = NULL;
config *configObject;

/*-------------------------- Configure Here --------------------------*/
/*
 * The following functions should be configured to your needs.
 */
int ts3plugin_init()
{
	char configPath[PATH_BUFSIZE];
	ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);
	configObject = new config(QString::fromUtf8(configPath) + CONFIG_FILE);

	return 0;
}

void ts3plugin_shutdown()
{
	if (configObject)
	{
		configObject->close();
		delete configObject;
		configObject = NULL;
	}

	if (pluginID)
	{
		free(pluginID);
		pluginID = NULL;
	}
}

enum
{
	MENU_ID_GLOBAL_SETTINGS = 1
};

void ts3plugin_initMenus(struct PluginMenuItem ***menuItems, char **menuIcon)
{
	BEGIN_CREATE_MENUS(1);
	CREATE_MENU_ITEM(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_SETTINGS, "Settings", "Settings.svg");
	END_CREATE_MENUS;
	*menuIcon = (char *)malloc(PLUGIN_MENU_BUFSZ * sizeof(char));
	_strcpy(*menuIcon, PLUGIN_MENU_BUFSZ, "Information.svg");
}

void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID)
{
	switch (type)
	{
	case PLUGIN_MENU_TYPE_GLOBAL:
		switch (menuItemID)
		{
		case MENU_ID_GLOBAL_SETTINGS:
			if (configObject->isVisible())
			{
				configObject->raise();
				configObject->activateWindow();
			}
			else
				configObject->show();
			break;
		}
		break;
	}
}

/*-------------------------- DON'T TOUCH --------------------------*/
/*
 * Those functions are setup nicely and
 * should be configured using the definitions.hpp file.
 */
const char *ts3plugin_name()
{
	return PLUGIN_NAME;
}

const char *ts3plugin_version()
{
	return PLUGIN_VERSION;
}

int ts3plugin_apiVersion()
{
	return PLUGIN_API_VERSION;
}

const char *ts3plugin_author()
{
	return PLUGIN_AUTHOR;
}

const char *ts3plugin_description()
{
	return PLUGIN_DESCRIPTION;
}

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs)
{
	ts3Functions = funcs;
}

int ts3plugin_offersConfigure()
{
	return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;
}

void ts3plugin_configure(void *handle, void *qParentWidget)
{
	if (configObject->isVisible())
	{
		configObject->raise();
		configObject->activateWindow();
	}
	else
	{
		configObject->show();
	}
}

void ts3plugin_registerPluginID(const char *id)
{
	const size_t sz = strlen(id) + 1;
	pluginID = (char *)malloc(sz * sizeof(char));
	_strcpy(pluginID, sz, id);
}

void ts3plugin_freeMemory(void *data)
{
	free(data);
}

/*-------------------------- OTHER STUFF --------------------------*/
/*
 * Those functions are not used in this demo plugin.
 */

std::string resolveSetting(int type, const QString input, uint64 sCHID, anyID id = NULL)
{
	if (type == 0)
	{ // SERVER
		// public_defenitions
		if (!QString::compare(input, QString("name"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_NAME, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Name:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("welcomemessage"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_WELCOMEMESSAGE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Welcome Message:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("platform"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_PLATFORM, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Platform:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("version"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_VERSION, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Version:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("created"), Qt::CaseInsensitive))
		{
			int val;
			if (ts3Functions.getServerVariableAsInt(sCHID, VIRTUALSERVER_CREATED, &val) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			QDateTime timestamp;
			timestamp.setTime_t(val);
			QString value;
			if (val == 0)
				value = "The database carries no entry!";
			else
				value = timestamp.toString(Qt::SystemLocaleShortDate);
			std::string ret = "[i]Created:[/i] " + std::string(value.toUtf8().constData());
			return ret;
		}
		if (!QString::compare(input, QString("codec_encryption_mode"), Qt::CaseInsensitive))
		{
			int val;
			if (ts3Functions.getServerVariableAsInt(sCHID, VIRTUALSERVER_CODEC_ENCRYPTION_MODE, &val) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Codec Encryption Mode:[/i] " + std::string(val == 0 ? "ENCRYPTION_PER_CHANNEL" : (val == 1 ? "ENCRYPTION_FORCED_OFF" : "ENCRYPTION_FORCED_ON"));
			return ret;
		}
		if (!QString::compare(input, QString("unique_identifier"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_UNIQUE_IDENTIFIER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Unique Identifier:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		// public_rare_definitions
		if (!QString::compare(input, QString("hostmessage"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTMESSAGE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostmessage:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostmessage_mode"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTMESSAGE_MODE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostmessage Mode:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("default_server_group"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_DEFAULT_SERVER_GROUP, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Default Server Group:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("default_channel_group"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_DEFAULT_CHANNEL_GROUP, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Default Channel Group:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("default_channel_admin_group"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_DEFAULT_CHANNEL_ADMIN_GROUP, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Default Channel Admin Group:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostbanner_url"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTBANNER_URL, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostbanner URL:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostbanner_gfx_url"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTBANNER_GFX_URL, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostbanner GFX URL:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostbanner_gfx_interval"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTBANNER_GFX_INTERVAL, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostbanner GFX Interval:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("priority_speaker_dimm_modificator"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_PRIORITY_SPEAKER_DIMM_MODIFICATOR, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Priority Speaker Dimm Modificator:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostbutton_tooltip"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTBUTTON_TOOLTIP, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostbutton Tooltip:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostbutton_url"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTBUTTON_URL, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostbutton URL:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostbutton_gfx_url"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTBUTTON_GFX_URL, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostbutton GFX URL:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("name_phonetic"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_NAME_PHONETIC, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Name Phonetic:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("icon_id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_ICON_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Icon ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("reserved_slots"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_RESERVED_SLOTS, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Reserved Slots:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("ask_for_privilegekey"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_ASK_FOR_PRIVILEGEKEY, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Ask For Privilegekey:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("hostbanner_mode"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_HOSTBANNER_MODE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Hostbanner Mode:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("channel_temp_delete_delay_default"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_CHANNEL_TEMP_DELETE_DELAY_DEFAULT, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Channel Temp Delete Delay Default:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("nickname"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getServerVariableAsString(sCHID, VIRTUALSERVER_NICKNAME, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Nickname:[/i] " + std::string(name);
			free(name);
			return ret;
		}
	}
	if (type == 1)
	{ // CHANNEL
		if (!QString::compare(input, QString("name"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_NAME, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Name:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("topic"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_TOPIC, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Topic:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("codec"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_CODEC, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Codec:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("codec_quality"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_CODEC_QUALITY, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Codec Quality:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("maxclients"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_MAXCLIENTS, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Max Clients:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("maxfamilyclients"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_MAXFAMILYCLIENTS, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Max Family Clients:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("order"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_ORDER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Order:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("permanent?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_PERMANENT, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Permanent:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("semi_permanent?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_SEMI_PERMANENT, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Semi Permanent:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("default?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_DEFAULT, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Default:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("password?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_PASSWORD, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Requires Password:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("codec_latency_factor"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_CODEC_LATENCY_FACTOR, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Codec Latency Factor:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("codec_is_unencrypted"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_CODEC_IS_UNENCRYPTED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Codec Is Unencrypted:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("unique_identifier"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_UNIQUE_IDENTIFIER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Unique Identifier:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		// public_rare_definitions
		if (!QString::compare(input, QString("maxclients_unlimited?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_MAXCLIENTS_UNLIMITED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Maxclients Unlimited:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("maxfamilyclients_unlimited?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_MAXFAMILYCLIENTS_UNLIMITED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Maxfamilyclients Unlimited:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("maxfamilyclients_inherited?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_MAXFAMILYCLIENTS_INHERITED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Maxfamilyclients Inherited:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("are_subscribed?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FLAG_ARE_SUBSCRIBED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Are Subscribed:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("needed_talk_power"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_NEEDED_TALK_POWER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Needed Talk Power:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("forced_silence"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_FORCED_SILENCE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Forced Silence:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("name_phonetic"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_NAME_PHONETIC, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Name Phonetic:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("icon_id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_ICON_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Icon ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("banner_gfx_url"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_BANNER_GFX_URL, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Banner GFX URL:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("banner_mode"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_BANNER_MODE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Banner Mode:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("permission_hints"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getChannelVariableAsString(sCHID, id, CHANNEL_PERMISSION_HINTS, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Permission Hints:[/i] " + std::string(name);
			free(name);
			return ret;
		}
	}
	if (type == 2)
	{ // CLIENT
		if (!QString::compare(input, QString("unique_identifier"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_UNIQUE_IDENTIFIER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Unique Identifier:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("nickname"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_NICKNAME, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Nickname:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("talking?"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_FLAG_TALKING, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Talking:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("input_muted"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_INPUT_MUTED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Input Muted:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("output_muted"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_OUTPUT_MUTED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Output Muted:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("outputonly_muted"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_OUTPUTONLY_MUTED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Output Only Muted:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("input_hardware"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_INPUT_HARDWARE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Input Hardware:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("output_hardware"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_OUTPUT_HARDWARE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Output Hardware:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("is_muted"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_IS_MUTED, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Locally Muted:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("is_recording"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_IS_RECORDING, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Recording:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		// public_rare_definitions
		if (!QString::compare(input, QString("database_id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_DATABASE_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Database ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("channel_group_id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_CHANNEL_GROUP_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Channel Group ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("servergroups"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_SERVERGROUPS, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Servergroups:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("away"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_AWAY, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Away:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("away_message"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_AWAY_MESSAGE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Away Message:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("type"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_TYPE, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Type:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("flag_avatar"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_FLAG_AVATAR, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Flag Avatar:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("talk_power"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_TALK_POWER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Talk Power:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("talk_request"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_TALK_REQUEST, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Talk Request:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("talk_request_msg"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_TALK_REQUEST_MSG, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Talk Request Message:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("description"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_DESCRIPTION, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Description:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("is_talker"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_IS_TALKER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Talker:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("is_priority_speaker"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_IS_PRIORITY_SPEAKER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Priority Speaker:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("unread_messages"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_UNREAD_MESSAGES, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Unread Messages:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("nickname_phonetic"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_NICKNAME_PHONETIC, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Nickname Phonetic:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("needed_serverquery_view_power"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_NEEDED_SERVERQUERY_VIEW_POWER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Needed Serverquery View Power:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("icon_id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_ICON_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Icon ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("is_channel_commander"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_IS_CHANNEL_COMMANDER, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Is Channel Commander:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("country"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_COUNTRY, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Country:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("channel_group_inherited_channel_id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_CHANNEL_GROUP_INHERITED_CHANNEL_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Channel Group Inherited Channel ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("badges"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_BADGES, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Badges:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("myteamspeak_id"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_MYTEAMSPEAK_ID, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]MyTeamSpeak ID:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("integrations"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_INTEGRATIONS, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Integrations:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("active_integrations_info"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_ACTIVE_INTEGRATIONS_INFO, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Active Integrations Info:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("myts_avatar"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_MYTS_AVATAR, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]MyTS Avatar:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("signed_badges"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_SIGNED_BADGES, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Signed Badges:[/i] " + std::string(name);
			free(name);
			return ret;
		}
		if (!QString::compare(input, QString("permission_hints"), Qt::CaseInsensitive))
		{
			char *name = NULL;
			if (ts3Functions.getClientVariableAsString(sCHID, id, CLIENT_PERMISSION_HINTS, &name) != ERROR_ok)
			{
				printf("Error getting variable.\n");
				return std::string("");
			}
			std::string ret = "[i]Permission Hints:[/i] " + std::string(name);
			free(name);
			return ret;
		}
	}
	return std::string("missing");
}

void resetConf()
{
	if (configObject)
	{
		configObject->close();
		delete configObject;
		configObject = NULL;
	}
	char configPath[PATH_BUFSIZE];
	ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);
	configObject = new config(QString::fromUtf8(configPath) + CONFIG_FILE);
	configObject->show();
}

/*
 * Implement the following three functions when the plugin should display a line in the server/channel/client info.
 * If any of ts3plugin_infoTitle, ts3plugin_infoData or ts3plugin_freeMemory is missing, the info text will not be displayed.
 */

/* Static title shown in the left column in the info frame */
const char *ts3plugin_infoTitle()
{
	return PLUGIN_NAME;
}

/*
 * Dynamic content shown in the right column in the info frame. Memory for the data string needs to be allocated in this
 * function. The client will call ts3plugin_freeMemory once done with the string to release the allocated memory again.
 * Check the parameter "type" if you want to implement this feature only for specific item types. Set the parameter
 * "data" to NULL to have the client ignore the info data.
 */
void ts3plugin_infoData(uint64 sCHID, uint64 id, enum PluginItemType type, char **data)
{
	std::string val = "";
	switch (type)
	{
	case PLUGIN_SERVER:
		if (!configObject->getConfigOption("server_enabled").toBool())
		{
			data = NULL;
			return;
		}
		for (int i = 0; i < configObject->m_ui->serverList->count(); i++)
		{
			QString confName = "server_" + QString(i);
			QString conf = configObject->getConfigOption(confName).toString();
			if (configObject->getConfigOption(QString("server_") + conf).toInt() != 2)
				continue;
			else
				val += resolveSetting(0, conf, sCHID) + "\n";
		}
		*data = (char *)malloc(strlen(val.c_str()) + 1);
		strcpy(*data, val.c_str());
		break;
	case PLUGIN_CHANNEL:
		if (!configObject->getConfigOption("channel_enabled").toBool())
		{
			data = NULL;
			return;
		}
		for (int i = 0; i < configObject->m_ui->channelList->count(); i++)
		{
			QString confName = "channel_" + QString(i);
			QString conf = configObject->getConfigOption(confName).toString();
			if (configObject->getConfigOption(QString("channel_") + conf).toInt() != 2)
				continue;
			else
				val += resolveSetting(1, conf, sCHID, id) + "\n";
		}
		*data = (char *)malloc(strlen(val.c_str()) + 1);
		strcpy(*data, val.c_str());
		break;
	case PLUGIN_CLIENT:
		if (!configObject->getConfigOption("client_enabled").toBool())
		{
			data = NULL;
			return;
		}
		for (int i = 0; i < configObject->m_ui->clientList->count(); i++)
		{
			QString confName = "client_" + QString(i);
			QString conf = configObject->getConfigOption(confName).toString();
			if (configObject->getConfigOption(QString("client_") + conf).toInt() != 2)
				continue;
			else
				val += resolveSetting(2, conf, sCHID, id) + "\n";
		}
		*data = (char *)malloc(strlen(val.c_str()) + 1);
		strcpy(*data, val.c_str());
		break;
	default:
		data = NULL; // Ignore
		return;
	}
}