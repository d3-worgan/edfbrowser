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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPIKEFILTER_SPIKE_NO      0
#define SPIKEFILTER_SPIKE_ONSET   1
#define SPIKEFILTER_SPIKE_STOP    2




struct spike_filter_settings{
                       int sf;
                       int holdoff;
                       int holdoff_sav;
                       int holdoff_set;
                       int holdoff_ms;
                       int cutoff;
                       int cutoff_sav;
                       int cutoff_set;
                       double array[256];
                       double array_sav[256];
                       int idx;
                       int idx_sav;
                       int n_max;
                       double smpl_base;
                       double smpl_base_sav;
                       double velocity;
                       int polarity;
                       int polarity_sav;
                       int second_flank_det;
                       int second_flank_det_sav;
                       int run_in;
                       int run_in_sav;
                       int *pd_sig;
                       void (*callback_func)(void);
                      };



struct spike_filter_settings * create_spike_filter(int, double, int, int *, void (*)(void));
double run_spike_filter(double, struct spike_filter_settings *);
void free_spike_filter(struct spike_filter_settings *);
void reset_spike_filter(struct spike_filter_settings *);
struct spike_filter_settings * create_spike_filter_copy(struct spike_filter_settings *);
void spike_filter_save_buf(struct spike_filter_settings *);
void spike_filter_restore_buf(struct spike_filter_settings *);


#endif


