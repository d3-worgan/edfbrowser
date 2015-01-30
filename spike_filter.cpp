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




#include "spike_filter.h"




struct spike_filter_settings * create_spike_filter(int sf, double sv, int ho, void (*callback_func_handler)(void))
{
  struct spike_filter_settings *st;

  if((sf < 4000) || (sf > 128000)) return NULL;

  if((sv < 0.0001) || (sv > 10E9))  return NULL;

  if((ho < 10) || (ho > 1000))  return NULL;

  st = (struct spike_filter_settings *) calloc(1, sizeof(struct spike_filter_settings));
  if(st==NULL)  return(NULL);

  st->sf = sf;    // samplefrequency

  st->n_max = sf / 2000;  // Velocity is measured between sample n and sample n + n_max (tdiff is 0.5 mSec.)

  st->cutoff_set = sf / 1000;  // set cutoff time to 1 milli-Second (ideally this should be equal to the spike width)

  st->holdoff_set = (sf * ho) / 1000;  // 100 milliSec. holdoff time (do not try to detect another spike during this time)

  st->holdoff_ms = ho;

  st->velocity = (sv / sf) * st->n_max;

  st->callback_func = callback_func_handler;

  return(st);
}


void reset_spike_filter(struct spike_filter_settings *st)
{
  st->cutoff_set = st->sf / 1000;

  st->holdoff_set = (st->sf * st->holdoff_ms) / 1000;

  st->cutoff = 0;

  st->holdoff = 0;

  st->second_flank_det = 0;

  st->polarity = 0;

  for(int i=0; i<st->n_max; i++)  st->array[i] = 0;

  st->idx = 0;

  st->run_in = 0;
}


struct spike_filter_settings * create_spike_filter_copy(struct spike_filter_settings *src)
{
  struct spike_filter_settings *settings;

  settings = (struct spike_filter_settings *) calloc(1, sizeof(struct spike_filter_settings));
  if(settings==NULL)
  {
    return(NULL);
  }
  memcpy(settings, src, sizeof(struct spike_filter_settings));

  return(settings);
}


double run_spike_filter(double x, struct spike_filter_settings *st)
{
  int p_det=0,
      pol=1;

  double d_tmp;

  if(st->run_in < st->n_max)
  {
    st->array[st->idx++] = x;

    st->idx %= st->n_max;

    st->run_in++;

    return x;
  }

  d_tmp = x - st->array[st->idx];  // check if there is a fast change between two samples

  if(d_tmp < 0.0)
  {
    d_tmp *= -1.0;  // we want an absolute value

    pol = -1;
  }

  if(d_tmp > st->velocity)  // change is fast enough to trigger a spike detect?
  {
    p_det = pol;
  }

  if(st->cutoff)  // are we in the spike suppression period?
  {
    st->cutoff--;

    st->holdoff--;

    if(p_det && (pol != st->polarity) && (!st->second_flank_det))  // did we detect the second flank of the spike?
    {
      if((st->cutoff_set > (st->sf / 500)) && ((st->cutoff_set - st->cutoff) > (st->sf / 1000)))  // adapt the suppression period to the measured pulsewidth
      {
        st->cutoff_set -= st->sf / 1000;  // decrease impulse suppression period with 1 milliSecond

        st->second_flank_det = 1;
      }
    }

    st->array[st->idx++] = x;

    st->idx %= st->n_max;

    return st->smpl_base;  // replace the sample with the baseline value because we are in the spike period
  }

  if(st->holdoff)  // are we in the holdoff period?
  {
    st->holdoff--;

    if(p_det && (pol != st->polarity) && (!st->second_flank_det))  // did we detect the second flank of the spike?
    {
      st->cutoff_set += st->sf / 1000;  // increase spike suppression period with 1 milliSecond

      if(st->cutoff_set > (st->sf / 20))
      {
        st->cutoff_set = st->sf / 20;  // limit the maximum suppression period to 50 milliSeconds
      }

      st->second_flank_det = 1;
    }

    st->array[st->idx++] = x;

    st->idx %= st->n_max;

    return st->array[st->idx];
  }

  if(p_det)  // We decided that we have found a new spike
  {
    st->smpl_base = st->array[st->idx];

    st->holdoff = st->holdoff_set;  // set the holdoff timer

    st->cutoff = st->cutoff_set;  // set the suppression period timer

    st->polarity = pol;

    st->second_flank_det = 0;

    st->array[st->idx++] = x;

    st->idx %= st->n_max;

    if(st->callback_func != NULL)  // If set, call the external function
    {
      st->callback_func();
    }

    return st->smpl_base;
  }

  st->array[st->idx++] = x;

  st->idx %= st->n_max;

  return st->array[st->idx];
}


void free_spike_filter(struct spike_filter_settings *st)
{
  free(st);
}


void spike_filter_save_buf(struct spike_filter_settings *st)
{
  st->holdoff_sav = st->holdoff;
  st->cutoff_sav = st->cutoff;
  for(int i=0; i<st->n_max; i++) st->array_sav[i] = st->array[i];
  st->idx_sav = st->idx;
  st->smpl_base_sav = st->smpl_base;
  st->polarity_sav = st->polarity;
  st->second_flank_det_sav = st->second_flank_det;
  st->run_in_sav = st->run_in;
}


void spike_filter_restore_buf(struct spike_filter_settings *st)
{
  st->holdoff = st->holdoff_sav;
  st->cutoff = st->cutoff_sav;
  for(int i=0; i<st->n_max; i++) st->array[i] = st->array_sav[i];
  st->idx = st->idx_sav;
  st->smpl_base = st->smpl_base_sav;
  st->polarity = st->polarity_sav;
  st->second_flank_det = st->second_flank_det_sav;
  st->run_in = st->run_in_sav;
}











