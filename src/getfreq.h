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

#ifndef GETFREQ_H
#define GETFREQ_H 1

void gf_init();
int gf_current(int core);
int gf_available(int core, char* out, int size);
void gf_get_frequency_label(int freq, char* out);
char* gf_freqa(int core, int index);
int gf_freqi(int core, int index);
int gf_number();


#endif
