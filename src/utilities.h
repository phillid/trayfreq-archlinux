/************************************************************************
 * This file is part of trayfreq.                                       *
 *                                                                      *
 * trayfreq is free software; you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published    *
 * by the Free Software Foundation; either version 3 of the License, or *
 * (at your option) any later version.                                  *
 *                                                                      *
 * trayfreq is distributed in the hope that it will be useful,	        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with trayfreq. If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

#ifndef UTILITIES_H
#define UTILITIES_H 1

#define PREFIX "NONE"

#include <glib.h>
//#include "config.h"

/* > 0 if a > b,  = 0 if a = b, < 0 if a < b*/
gint util_compare_gdouble(gdouble a, gdouble b);
const gchar* util_get_prefix();

#endif