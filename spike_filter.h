/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014, 2015 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/





#ifndef spike_filter_INCLUDED
#define spike_filter_INCLUDED



#include <stdlib.h>
#include <string.h>




struct spike_filter_settings{
                       double sf;
                       int holdoff;
                       int holdoff_set;
                       int holdoff_ms;
                       int cutoff;
                       int cutoff_set;
                       double smpl_1;
                       double smpl_2;
                       double smpl_base;
                       double velocity;
                       int polarity;
                       int p_det_1;
                       int second_flank_det;
                      };



struct spike_filter_settings * create_spike_filter(double, double, int);
double run_spike_filter(double, struct spike_filter_settings *);
void free_spike_filter(struct spike_filter_settings *);
void reset_spike_filter(struct spike_filter_settings *);
struct spike_filter_settings * create_spike_filter_copy(struct spike_filter_settings *);


#endif


