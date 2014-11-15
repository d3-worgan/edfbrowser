/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014 Teunis van Beelen
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




struct spike_filter_settings * create_spike_filter(double sf, double sv, int ho)
{
  struct spike_filter_settings *st;

  if(sf < 1000.0)  return NULL;

  if((sv < 0.0001) || (sv > 10E9))  return NULL;

  if((ho < 10) || (ho > 1000))  return NULL;

  st = (struct spike_filter_settings *) calloc(1, sizeof(struct spike_filter_settings));
  if(st==NULL)  return(NULL);

  st->sf = sf;    // samplefrequency

  st->cutoff_set = sf / 1000.0;  // set cutoff time to 1 milli-Second (ideally this should be equal to the pace pulsewidth)

  st->holdoff_set = sf / (1000.0 / (double)ho);  // 100 milliSec. holdoff time (do not try to detect another pace impulse during this time)

  st->holdoff_ms = ho;

  st->velocity = sv / sf;         // V per second (if the difference between two consecutive samples is more than
                                  //               "velocity", a pace impulse is detected)
  return(st);
}


void reset_spike_filter(struct spike_filter_settings *st)
{
  st->cutoff_set = st->sf / 1000.0;

  st->holdoff_set = st->sf / (1000.0 / (double)st->holdoff_ms);

  st->cutoff = 0;

  st->holdoff = 0;

  st->second_flank_det = 0;

  st->p_det_1 = 0;

  st->polarity = 0;
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

  d_tmp = x - st->smpl_1;  // check if there is a fast change between two consecutive samples

  if(d_tmp < 0.0)
  {
    d_tmp *= -1.0;  // we want an absolute value

    pol = -1;
  }

  if(d_tmp > st->velocity)  // change is fast enough to trigger a pace detect?
  {
    p_det = pol;
  }

  if(st->cutoff)  // are we in the pace impulse suppression period?
  {
    st->cutoff--;

    st->holdoff--;

    if(p_det && (pol != st->polarity) && (!st->second_flank_det))  // did we detect the second flank of the pace impulse?
    {
      if((st->cutoff_set > (st->sf / 500)) && ((st->cutoff_set - st->cutoff) > (st->sf / 1000)))  // adapt the suppression period to the measured pulsewidth
      {
        st->cutoff_set -= st->sf / 1000;  // decrease impulse suppression period with 1 milliSecond

        st->second_flank_det = 1;
      }
    }

    st->smpl_2 = st->smpl_1;

    st->smpl_1 = x;

    return st->smpl_base;  // replace the sample with the baseline value because we are in the pace impulse period
  }

  if(st->holdoff)  // are we in the holdoff period?
  {
    st->holdoff--;

    if(p_det && (pol != st->polarity) && (!st->second_flank_det))  // did we detect the second flank of the pace impulse?
    {
      st->cutoff_set += st->sf / 1000;  // increase impulse suppression period with 1 milliSecond

      if(st->cutoff_set > (st->sf / 20))
      {
        st->cutoff_set = st->sf / 20;  // limit the maximum suppression period to 50 milliSeconds
      }

      st->second_flank_det = 1;
    }

    st->smpl_2 = st->smpl_1;

    st->smpl_1 = x;

    return st->smpl_2;
  }

  if((p_det) && (st->p_det_1 == p_det))
  {
    st->smpl_base = st->smpl_2;

    st->holdoff = st->holdoff_set;  // set the holdoff timer

    st->cutoff = st->cutoff_set;  // set the suppression period timer

    st->polarity = pol;

    st->second_flank_det = 0;

    st->p_det_1 = 0;

    st->smpl_2 = st->smpl_1;

    st->smpl_1 = x;

    return st->smpl_base;
  }

  st->p_det_1 = p_det;

  st->smpl_2 = st->smpl_1;

  st->smpl_1 = x;

  return st->smpl_2;
}


void free_spike_filter(struct spike_filter_settings *st)
{
  free(st);
}











