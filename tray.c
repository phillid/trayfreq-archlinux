/************************************************************************
 * This file is part of trayfreq-archlinux.                             *
 *                                                                      *
 * trayfreq-archlinux is free software; you can redistribute it and/or  *
 * modify it under the terms of the GNU General Public License as       *
 * published by the Free Software Foundation; either version 3 of the   *
 * License, or (at your option) any later version.                      *
 *                                                                      *
 * trayfreq-archlinux is distributed in the hope that it will be useful,*
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with trayfreq-archlinux. If not, see                           *
 * <http://www.gnu.org/licenses/>.                                      *
 ************************************************************************/

#include "tray.h"
#include "widget_manager.h"
#include "freq_tray/getfreq.h"
#include "freq_tray/getcore.h"
#include "freq_tray/getgov.h"
#include "trayfreq_set/trayfreq_set_interface.h"
#include "bat_tray/bat_tray.h"
#include "defaults.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>

GtkStatusIcon* tray;
#define TOOLTIP_TEXT_SIZE 500
gchar tooltip_text[TOOLTIP_TEXT_SIZE];


/* 0 = nothing, 1 = was charging, 2 = was discharging */
//int state = 0;


GtkWidget* menu;
GSList* menu_items;
GtkWidget* checked_menu_item;

static void freq_menu_item_toggled(GtkCheckMenuItem* item, gpointer data)
{
	if(gtk_check_menu_item_get_active(item))
	{
		checked_menu_item = GTK_WIDGET(item);
		gint freq = GPOINTER_TO_INT(data);
		int i = 0;
		for(i = 0; i < gc_number(); ++i)
			si_freq(freq, i);
	}
}

static void gov_menu_item_toggled(GtkCheckMenuItem* item, gpointer data)
{
	if(gtk_check_menu_item_get_active(item))
	{
		checked_menu_item = GTK_WIDGET(item);
		gchar* gov = (gchar*)data;
		int i = 0;
		for(i = 0; i < gc_number(); ++i)
			si_gov(gov, i);
	}
}
/*
static gboolean governor_exists(gchar* governor)
{
	int i = 0;
	for(i = 0; i < gf_number(); ++i)
	{
		if(g_strcmp0(governor, gg_gov(0, i)) == 0)
			return TRUE;
	}
	return FALSE;
}*/

static void remove_menu_item(GtkWidget* menu_item, gpointer data)
{
	gtk_widget_destroy(menu_item);
}

static void tray_clear_menu()
{
	GtkContainer* cont = GTK_CONTAINER(menu);
	gtk_container_foreach(cont, remove_menu_item, NULL);
	menu_items = NULL;
}

static void tray_init_menu()
{
	menu = gtk_menu_new();
}

static void tray_generate_menu()
{
	tray_clear_menu();
	gg_init();

	gchar label[20];
	int i = 0;

	gchar current_governor[20];
	memset(current_governor, '\0', 20);
	gg_current(0, current_governor, 20);

	gint current_frequency = gf_current(0);

	/* append the frequencies */
	for(i = 0; i < gf_number(); ++i)
	{
		memset(label, '\0', 20);
		gf_get_frequency_label(gf_freqi(0, i), label);

		GtkWidget* item = gtk_radio_menu_item_new_with_label(menu_items, label);
		menu_items = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM (item));

		if(g_strcmp0(current_governor, "userspace") == 0 && gf_freqi(0, i) == current_frequency)
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);

		g_signal_connect(G_OBJECT(item), "toggled", GTK_SIGNAL_FUNC(freq_menu_item_toggled), GINT_TO_POINTER(gf_freqi(0, i)));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	}

	/* append the seperator */
	GtkWidget* seperator = gtk_separator_menu_item_new();
	gtk_menu_append(menu, seperator);

	/* append the governors*/
	for(i = 0; i < gg_number(); i++)
	{
		if(g_strcmp0(gg_gov(0, i), "userspace") == 0)
		continue;

		GtkWidget* item = gtk_radio_menu_item_new_with_label(menu_items, gg_gov(0, i));
		menu_items = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM (item));

		if(g_strcmp0(gg_gov(0, i), current_governor) == 0)
		{
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);
		}

		g_signal_connect(G_OBJECT(item), "toggled", GTK_SIGNAL_FUNC(gov_menu_item_toggled), gg_gov(0, i));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	}
	gtk_widget_show_all(menu);
}

static gboolean update_tooltip(GtkStatusIcon* status_icon,gint x,gint y,gboolean keyboard_mode,GtkTooltip* tooltip,gpointer data)
{
	gchar msg[500];
	gchar current_governer[20];
	gchar label[20];
	int i = 0;

	memset(msg, 0, 500);
	memset(current_governer, 0, 20);

	// Change governor based on battery state battery is discharging
	if ( get_battery_state() ==  STATE_DISCHARGING )
	{
		if(_DEFAULT_BAT_GOV)
		{
			for(i = 0; i < gc_number(); ++i)
				si_gov(_DEFAULT_BAT_GOV, i);
		}
	}
	/* battery is now charging (on ac) */
	else if ( get_battery_state() ==  STATE_CHARGING )
	{
		if(_DEFAULT_AC_GOV)
		{
			for(i = 0; i < gc_number(); ++i)
				si_gov(_DEFAULT_AC_GOV, i);
		}
	}

	gg_current(0, current_governer, 20);
	sprintf(msg+strlen(msg), _("Governor: %s\n"), current_governer);

	for(i = 0; i < gc_number(); ++i)
	{
		memset(label, 0, 20);
		gf_get_frequency_label(gf_current(i), label);
		sprintf(msg+strlen(msg), _("CPU%i: %s%s"), i, label, i == gc_number()-1 ? "" : "\n");
	}

	tray_set_tooltip(msg);
	gtk_tooltip_set_text(tooltip, tooltip_text);

	return TRUE;
}

static void popup_menu(GtkStatusIcon* statuc_icon,guint button,guint activate_time,gpointer data)
{
	tray_generate_menu();
	gtk_menu_popup(GTK_MENU(menu),NULL,NULL,gtk_status_icon_position_menu,tray,button,activate_time);
}

static void activate(GtkStatusIcon* statuc_icon,gpointer data)
{
	if(_DEFAULT_PROG)
		g_spawn_command_line_async(_DEFAULT_PROG, NULL);
}

static gboolean update_icon(gpointer user_data)
{
	tray_update_icon_percent();
	return TRUE;
}

void tray_init()
{
	// Set defaults
	int i = 0;
	if(_DEFAULT_GOV)
	{
		for(i = 0; i < gc_number(); ++i)
			si_gov(_DEFAULT_GOV, i);

	} else {
		for(i = 0; i < gc_number(); ++i)
			si_gov("ondemand", i);
	}

	if(_DEFAULT_FREQ)
	{
		for(i = 0; i < gc_number(); ++i)
		{
			si_freq(atoi(_DEFAULT_FREQ), i);
		}
	}

	tray = gtk_status_icon_new();
	gchar* icon_file = g_strconcat("/usr/share/trayfreq/cpufreq-0.png", NULL);
	gtk_status_icon_set_from_file(tray, icon_file);
	gtk_status_icon_set_has_tooltip (tray, TRUE);
	g_signal_connect(G_OBJECT(tray), "query-tooltip", GTK_SIGNAL_FUNC(update_tooltip), NULL);
	g_signal_connect(G_OBJECT(tray), "popup-menu", GTK_SIGNAL_FUNC(popup_menu), NULL);
	g_signal_connect(G_OBJECT(tray), "activate", GTK_SIGNAL_FUNC(activate), NULL);
	gtk_timeout_add(1000, update_icon, NULL);
	tray_init_menu();
}

void tray_set_tooltip(const gchar* msg)
{
	memset(tooltip_text, '\0', TOOLTIP_TEXT_SIZE);
	memmove(tooltip_text, msg, strlen(msg));
}

void tray_update_icon_percent()
{
	gulong max_frequency = gf_freqi(0, 0);
	gint adjusted_percent = 0;
	// If no governor, set percentage to 0. This if statement fixes an FPE a few lines down
	if (max_frequency == 0)
	{
		adjusted_percent = 0;
	} else {
		// Percentages need to be {25,50,75,100}. Round to one of these numbers.
		gint percent = (gf_current(0) * 100)/max_frequency;
		if(percent == 100) {
			adjusted_percent = 100;
		} else if(percent >= 65.5) {
			adjusted_percent = 75;
		} else if(percent >= 37.5) {
			adjusted_percent = 50;
		} else if(percent >= 12.5) {
			adjusted_percent = 25;
		} else {
			adjusted_percent = 0;
		}
	}

	/* convert the int to a string */
	gchar adjusted_percent_string[] = {'\0', '\0', '\0', '\0'};
	sprintf(adjusted_percent_string, "%i", adjusted_percent);

	gchar* file = g_strconcat("/usr/share/trayfreq/cpufreq-", adjusted_percent_string, ".png", NULL);
	gtk_status_icon_set_from_file(tray, file);

	g_free(file);
}

void tray_show()
{
	gtk_status_icon_set_visible(tray, TRUE);
}

void tray_hide()
{
	gtk_status_icon_set_visible(tray, FALSE);
}

gboolean tray_visible()
{
	return gtk_status_icon_get_visible(tray);
}

gboolean tray_embedded()
{
	return gtk_status_icon_is_embedded(tray);
}
