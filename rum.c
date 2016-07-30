/*
 * Rum Plugin - especially for pyrates
 *
 * Copyright (C) 2016 Morpheus Being <morpheus.being@riseup.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02111-1301, USA.
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include<stdio.h>
#include<stdlib.h>

/* Include glib.h for various types */
#include <glib.h>

/* This will prevent compiler errors in some instances and is better explained in the
 * how-to documents on the wiki */
#ifndef G_GNUC_NULL_TERMINATED
# if __GNUC__ >= 4
#  define G_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
# else
#  define G_GNUC_NULL_TERMINATED
# endif
#endif

/* This is the required definition of PURPLE_PLUGINS as required for a plugin,
 * but we protect it with an #ifndef because config.h may define it for us
 * already and this would cause an unneeded compiler warning. */
#ifndef PURPLE_PLUGINS
# define PURPLE_PLUGINS
#endif

/* Here we're including the necessary libpurple headers for this plugin.  Note
 * that we're including them in alphabetical order.  This isn't necessary but
 * we do this throughout our source for consistency. */
#include "cmds.h"
#include "debug.h"
#include "plugin.h"
#include "notify.h"
#include "version.h"

/* It's more convenient to type PLUGIN_ID all the time than it is to type
 * "core-commandexample", so define this convenience macro. */
#define PLUGIN_ID "rum"

/* Common practice in third-party plugins is to define convenience macros for
 * many of the fields of the plugin info struct, so we'll do that for the
 * purposes of demonstration. */
#define PLUGIN_AUTHOR "Morpheus Being <morpheus.being@riseup.net>"

/**
 * Initialized in the plugin_load() method, this allows us to keep a handle to 
 * ourself. Such a handle is needed to register for commands, so that libpurple
 * has a handle to the plugin that registered for the command 
 */ 
static PurplePlugin *rum = NULL;

/**
 * Used to hold a handle to the commands we register. Holding this handle
 * allows us to unregister the commands at a later time. 
 */
static PurpleCmdId log_command_id, rumi_command_id, rumd_command_id, rumc_command_id;

/**
 * The callback function for our /log command. This function simply prints
 * whatever was entered as the argument to the debug command into the debug 
 * log. 
 *
 * @param conv The conversation that the command occurred in
 * @param cmd The exact command that was entered
 * @param args The args that were passed with the command
 * @param error ?Any errors that occurred?
 * @param data Any special user-defined data that was assigned during 
 *             cmd_register
 */
static PurpleCmdRet 
log_cb(PurpleConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
  purple_debug_misc(PLUGIN_ID, "log_cb called\n");
  purple_debug_misc(PLUGIN_ID, "message = %s\n", args[0]);

  return PURPLE_CMD_RET_OK;

}

/**
 * The callback function for our /notify command. This function simply pops up
 * a notification with the word entered as the argument to the command 
 *
 * @param conv The conversation that the command occurred in
 * @param cmd The exact command that was entered
 * @param args The args that were passed with the command
 * @param error ?Any errors that occurred?
 * @param data Any special user-defined data that was assigned during 
 *             cmd_register
 */
static PurpleCmdRet 
rumd_cb(PurpleConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
  gchar *msg;
  
  msg = g_strdup_printf("Cap'n orders: decrease rum ration for the Pyrate %s", args[0]);

  switch(purple_conversation_get_type(conv))
      {
          case PURPLE_CONV_TYPE_IM:
              purple_conv_im_send(PURPLE_CONV_IM(conv), msg);
              break;
          case PURPLE_CONV_TYPE_CHAT:
              purple_conv_chat_send(PURPLE_CONV_CHAT(conv), msg);
              break;
          default:
              break;
      }
  g_free(msg);

  return PURPLE_CMD_RET_OK;

}

static PurpleCmdRet 
rumi_cb(PurpleConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
  gchar *msg;
  
  msg = g_strdup_printf("Cap'n orders: increase rum ration for the Pyrate %s ", args[0]);

  switch(purple_conversation_get_type(conv))
      {
          case PURPLE_CONV_TYPE_IM:
              purple_conv_im_send(PURPLE_CONV_IM(conv), msg);
              break;
          case PURPLE_CONV_TYPE_CHAT:
              purple_conv_chat_send(PURPLE_CONV_CHAT(conv), msg);
              break;
          default:
              break;
      }
  g_free(msg);

  return PURPLE_CMD_RET_OK;

}

static PurpleCmdRet 
rumc_cb(PurpleConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
  gchar *msg;
  
  msg = g_strdup_printf("Cap'n orders: cancel rum ration for the Pyrate %s until further notice!", args[0]);

  switch(purple_conversation_get_type(conv))
      {
          case PURPLE_CONV_TYPE_IM:
              purple_conv_im_send(PURPLE_CONV_IM(conv), msg);
              break;
          case PURPLE_CONV_TYPE_CHAT:
              purple_conv_chat_send(PURPLE_CONV_CHAT(conv), msg);
              break;
          default:
              break;
      }
  g_free(msg);

  return PURPLE_CMD_RET_OK;

}

/**
 * Because we added a pointer to this function in the load section of our 
 * PurplePluginInfo, this function is called when the plugin loads.  
 * Here we're using it to show off the capabilities of the
 * command API by registering a few commands. We return TRUE to tell libpurple 
 * it's safe to continue loading.
 */
static gboolean
plugin_load(PurplePlugin *plugin)
{
  // Declare all vars up front. Avoids warnings on some compilers 
  gchar *log_help = NULL;
  gchar *rum_help = NULL;
  
  // Save a handle to ourself for later
  rum = plugin;

  // Help message for log command, in the correct format 
  log_help = "log &lt;log message here&gt;:  Prints a message to the debug log.";

  // Register a command to allow a user to enter /log some message and have
  // that message stored to the log. This command runs with default priority,
  // can only be used in a standard chat message, not while in group chat
  // mode
  log_command_id = purple_cmd_register 
    ("log",                         /* command name */ 
     "s",                           /* command argument format */
     PURPLE_CMD_P_DEFAULT,          /* command priority flags */  
     PURPLE_CMD_FLAG_IM |
       PURPLE_CMD_FLAG_CHAT,            /* command usage flags */
     PLUGIN_ID,                     /* Plugin ID */
     log_cb,                        /* Name of the callback function */
     log_help,                      /* Help message */
     NULL                           /* Any special user-defined data */
     );
        

  // Help message for notify command, in the correct format
  rum_help = "rumd/rumi &lt;user &gt;:  Message to reduce/increase/cancel rum ration for a specified user.";

  // Register a command to allow a user to enter /notify some word and have
  // that word notified back to them. This command runs with high priority,
  // and can be used in both group and standard chat messages 
  rumd_command_id = purple_cmd_register
    ("rumd",                        /* command name */ 
     "w",                           /* command argument format */
     PURPLE_CMD_P_HIGH,             /* command priority flags */  
     PURPLE_CMD_FLAG_IM | 
       PURPLE_CMD_FLAG_CHAT,        /* command usage flags */
     PLUGIN_ID,                     /* Plugin ID */
     rumd_cb,                       /* Callback function */
     rum_help,                      /* Help message */
     NULL                           /* Any special user-defined data */
     );
 
  rumi_command_id = purple_cmd_register
    ("rumi",                      /* command name */ 
     "w",                           /* command argument format */
     PURPLE_CMD_P_HIGH,             /* command priority flags */  
     PURPLE_CMD_FLAG_IM | 
       PURPLE_CMD_FLAG_CHAT,        /* command usage flags */
     PLUGIN_ID,                     /* Plugin ID */
     rumi_cb,                       /* Callback function */
     rum_help,                      /* Help message */
     NULL                           /* Any special user-defined data */
     );

  rumc_command_id = purple_cmd_register
    ("rumc",                        /* command name */ 
     "w",                           /* command argument format */
     PURPLE_CMD_P_HIGH,             /* command priority flags */  
     PURPLE_CMD_FLAG_IM | 
       PURPLE_CMD_FLAG_CHAT,        /* command usage flags */
     PLUGIN_ID,                     /* Plugin ID */
     rumc_cb,                       /* Callback function */
     rum_help,                      /* Help message */
     NULL                           /* Any special user-defined data */
     );

  /* Just return true to tell libpurple to finish loading. */
  return TRUE;
}

/**
 * Because we added a pointer to this function in the unload section of our 
 * PurplePluginInfo, this function is called when the plugin unloads.  
 * Here we're using it to show off the capabilities of the
 * command API by unregistering a few commands. We return TRUE to tell libpurple 
 * it's safe to continue unloading.
 */
static gboolean
plugin_unload(PurplePlugin *plugin)
{
  purple_cmd_unregister(log_command_id);
  purple_cmd_unregister(rumd_command_id);
  purple_cmd_unregister(rumi_command_id);
  purple_cmd_unregister(rumc_command_id);

  /* Just return true to tell libpurple to finish unloading. */
  return TRUE;
}

/**
 * Struct used to let Pidgin understand our plugin
 */
static PurplePluginInfo info = {
        PURPLE_PLUGIN_MAGIC,        /* magic number */
        PURPLE_MAJOR_VERSION,       /* purple major */
        PURPLE_MINOR_VERSION,       /* purple minor */
        PURPLE_PLUGIN_STANDARD,     /* plugin type */
        NULL,                       /* UI requirement */
        0,                          /* flags */
        NULL,                       /* dependencies */
        PURPLE_PRIORITY_DEFAULT,    /* priority */

        PLUGIN_ID,                  /* id */
        "Rum",                      /* name */
        "0.2",                      /* version */
        "Rum ration increase/decrease/cancel for a specified user",      /* summary */
        "rum ration increase/decreate/cancel for a specified user",      /* description */
        PLUGIN_AUTHOR,              /* author */
        "http://pidgin.im",         /* homepage */

        plugin_load,                /* load */
        plugin_unload,              /* unload */
        NULL,                       /* destroy */

        NULL,                       /* ui info */
        NULL,                       /* extra info */
        NULL,                       /* prefs info */
        NULL,                       /* actions */
        NULL,                       /* reserved */
        NULL,                       /* reserved */
        NULL,                       /* reserved */
        NULL                        /* reserved */
};

static void
init_plugin(PurplePlugin *plugin)
{
}

PURPLE_INIT_PLUGIN(rum, init_plugin, info)

