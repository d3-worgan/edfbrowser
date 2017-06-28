/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017 Teunis van Beelen
*
* Email: teuniz@gmail.com
*
**************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*
* Inspired by:
*
* - Subtraction Method For Powerline Interference Removing From ECG
*   Chavdar Levkov, Georgy Mihov, Ratcho Ivanov, Ivan K. Daskalov
*   Ivaylo Christov, Ivan Dotsinsky
*
* - Removal of power-line interference from the ECG: a review of the
*   subtraction procedure
*   Chavdar Levkov, Georgy Mihov, Ratcho Ivanov, Ivan Daskalov,
*   Ivaylo Christov and Ivan Dotsinsky
*
* - Accuracy of 50 Hz interference subtraction from an electrocardiogram
*   I. A, Dotsinsky I.K. Daskalov
*
* - Dynamic powerline interference subtraction from biosignals
*   Ivaylo I. Christov
*
*
* The subtraction method extracts the powerline interference noise during a
* a linear region between two consecutive QRS complexes and stores it in a buffer.
* The reference noise from the buffer is used to subtract it from the signal outside
* the linear region i.e. during the QRS complex.
* This method only works correctly when the ratio of the samplefrequency and the
* powerline frequency is an integer multiple.
* In case they are synchronized, this method will remove also the harmonics of the
* powerline frequency. In that case extra notch-filters for the harmonics are
* not necessary. The advantage of this method is that it will not cause ringing
* in the waveform of the QRS complex (like notch-filters do).
*
***************************************************************************
*/




#include "plif_ecg_subtract_filter.h"


/*
 *
 * sf: samplefrequency (must be >= 500Hz and must be an integer multiple of the powerline frequency)
 *
 * pwlf: powerline frequency (must be 50Hz or 60Hz)
 *
 * lt: linear region threshold (default is 100uV) used to detect linear region
 *     between two consecutive QRS complexes
 *
 */
struct plif_subtract_filter_settings * plif_create_subtract_filter(int sf, int pwlf, double lt)
{
  int i;

  struct plif_subtract_filter_settings *st;

/* perform some sanity checks */
  if(sf < 500)  return NULL;  /* we need at least the samplefrequency considered the "gold standard" */

  if((pwlf != 50) && (pwlf != 60))  return NULL;  /* powerline frequency must be either 50 or 60Hz */

  if(sf % pwlf)  return NULL;  /* ratio between the samplefrequency and the powerline frequency must be an integer multiple */

  if((lt < 1) || (lt > 100000))  return NULL;  /* range for the linear detection threshold */

  st = (struct plif_subtract_filter_settings *) calloc(1, sizeof(struct plif_subtract_filter_settings));
  if(st==NULL)  return NULL;

  st->sf = sf;
  st->tpl = sf / pwlf;  /* the number of samples in one cycle of the powerline frequency */
  st->ravg_idx = 0;
  st->buf_idx = 0;
  st->linear = 0;
  st->incr = 0;
  st->fpl_corr = 5;  /* not used for now */
  st->shift = 12;  /* not used for now */
  st->linear_threshold = lt;  /* the threshold to detect the linear region */
  st->ravg_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ravg_buf == NULL) /* buffer for the running average filter */
  {
    free(st);
    return NULL;
  }
  st->ref_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ref_buf == NULL)  /* buffer for the reference noise, used to be subtracted from the ECG signal */
  {
    free(st->ravg_buf);
    free(st);
    return NULL;
  }
  st->ref_buf_new = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ref_buf_new == NULL)
  {
    free(st->ref_buf);
    free(st->ravg_buf);
    free(st);
    return NULL;
  }
  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->ravg_noise_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->ravg_noise_buf[i] == NULL)  /* buffers used for the noise extraction */
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st->ref_buf_new);
      free(st);
      return NULL;
    }
  }
  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->input_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->input_buf[i] == NULL)  /* inputbuffers used for detecting the linear region */
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st->ref_buf_new);
      free(st);
      return NULL;
    }
  }

  return st;
}


double plif_run_subtract_filter(double new_input, struct plif_subtract_filter_settings *st)
{
  int i, j, n, p;

  double ravg_val, fd_max, fd_min;

  if(st == NULL)
  {
    return 0;
  }

  st->input_buf[st->buf_idx][st->ravg_idx] = new_input;

/* running average filter */
  st->ravg_buf[st->ravg_idx] = new_input;

  ravg_val = 0;

  for(i=0; i<st->tpl; i++)
  {
    ravg_val += st->ravg_buf[i];
  }

  ravg_val /= st->tpl;

  st->ravg_noise_buf[st->buf_idx][st->ravg_idx] = new_input - ravg_val;  /* store the noise extracted from the signal into the buffers */

  p = st->ravg_idx;

  if(++st->ravg_idx >= st->tpl)  /* buffer full? */
  {
    st->ravg_idx = 0;

    st->buf_idx++;  /* increment the index */
    st->buf_idx %= PLIF_NBUFS; /* check boundary and roll-over if necessary */

    for(j=0, st->linear=1; j<st->tpl; j++)
    {
      fd_max = -1e9;
      fd_min = 1e9;

      for(i=0; i<PLIF_NBUFS; i++)  /* check all buffers for their max and min values */
      {                             /* distance between the 1th differences equals tpl in order to exclude the noise from the detection */
        if(st->input_buf[i][j] > fd_max)  fd_max = st->input_buf[i][j];

        if(st->input_buf[i][j] < fd_min)  fd_min = st->input_buf[i][j];
      }

      if((fd_max - fd_min) > st->linear_threshold)  /* if we exceed the threshold, we are not in a linear region */
      {
        st->linear = 0;

        break;
      }
    }

    if(st->linear)  /* are we in a linear region? */
    {
      for(j=0, n=0; j<PLIF_NBUFS; j++)  /* average the buffers containing the extracted noise */
      {
        if(!n)
        {
          for(i=0; i<st->tpl; i++)
          {
            st->ref_buf_new[i] = st->ravg_noise_buf[j][i];
          }
        }

        else
        {
          for(i=0; i<st->tpl; i++)
          {
            st->ref_buf_new[i] += st->ravg_noise_buf[j][i];
          }
        }

        n++;
      }

      for(i=0; i<st->tpl; i++)
      {
        st->ref_buf_new[i] /= n;  /* calculate the average */
      }

      memcpy(st->ref_buf, st->ref_buf_new, sizeof(double) * st->tpl);
    }
  }

  return new_input - st->ref_buf[p];
}


struct plif_subtract_filter_settings * plif_subtract_filter_create_copy(struct plif_subtract_filter_settings *st_ori)
{
  int i;

  struct plif_subtract_filter_settings *st;

  if(st_ori == NULL)
  {
    return NULL;
  }

  st = (struct plif_subtract_filter_settings *) calloc(1, sizeof(struct plif_subtract_filter_settings));
  if(st==NULL)  return NULL;

  *st = *st_ori;

  st->ravg_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ravg_buf == NULL)
  {
    free(st);
    return NULL;
  }
  memcpy(st->ravg_buf, st_ori->ravg_buf, sizeof(double) * st->tpl);

  st->ref_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ref_buf == NULL)
  {
    free(st->ravg_buf);
    free(st);
    return NULL;
  }
  memcpy(st->ref_buf, st_ori->ref_buf, sizeof(double) * st->tpl);

  st->ref_buf_new = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ref_buf_new == NULL)
  {
    free(st->ref_buf);
    free(st->ravg_buf);
    free(st);
    return NULL;
  }
  memcpy(st->ref_buf_new, st_ori->ref_buf_new, sizeof(double) * st->tpl);

  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->ravg_noise_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->ravg_noise_buf[i] == NULL)
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st->ref_buf_new);
      free(st);
      return NULL;
    }
    memcpy(st->ravg_noise_buf[i], st_ori->ravg_noise_buf[i], sizeof(double) * st->tpl);
  }

  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->input_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->input_buf[i] == NULL)
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st->ref_buf_new);
      free(st);
      return NULL;
    }
    memcpy(st->input_buf[i], st_ori->input_buf[i], sizeof(double) * st->tpl);
  }

  return st;
}


void plif_free_subtract_filter(struct plif_subtract_filter_settings *st)
{
  int i;

  if(st == NULL)
  {
    return;
  }

  free(st->ravg_buf);
  for(i=0; i<PLIF_NBUFS; i++)
  {
    free(st->ravg_noise_buf[i]);
    free(st->input_buf[i]);
  }
  free(st->ref_buf);
  free(st->ref_buf_new);
  free(st);
}


void plif_reset_subtract_filter(struct plif_subtract_filter_settings *st, double reference)
{
  int i, j;

  if(st == NULL)
  {
    return;
  }

  st->ravg_idx = 0;
  st->buf_idx = 0;
  st->linear = 0;
  st->incr = 0;

  for(j=0; j<st->tpl; j++)
  {
    st->ravg_buf[j] = reference;
  }

  for(i=0; i<PLIF_NBUFS; i++)
  {
    memset(st->ravg_noise_buf[i], 0, sizeof(double) * st->tpl);

    for(j=0; j<st->tpl; j++)
    {
      st->input_buf[i][j] = reference;
    }
  }

  memset(st->ref_buf, 0, sizeof(double) * st->tpl);
  memset(st->ref_buf_new, 0, sizeof(double) * st->tpl);
}


void plif_subtract_filter_state_copy(struct plif_subtract_filter_settings *dest, struct plif_subtract_filter_settings *src)
{
  int i;

  if((dest == NULL) || (src == NULL))  return;

  if(dest->sf != src->sf)  return;

  if(dest->tpl != src->tpl)  return;

  dest->ravg_idx = src->ravg_idx;
  dest->buf_idx = src->buf_idx;
  dest->fpl_corr = src->fpl_corr;
  dest->linear_threshold = src->linear_threshold;

  memcpy(dest->ravg_buf, src->ravg_buf, sizeof(double) * dest->tpl);
  memcpy(dest->ref_buf, src->ref_buf, sizeof(double) * dest->tpl);
  memcpy(dest->ref_buf_new, src->ref_buf_new, sizeof(double) * dest->tpl);

  for(i=0; i<PLIF_NBUFS; i++)
  {
    memcpy(dest->ravg_noise_buf[i], src->ravg_noise_buf[i], sizeof(double) * dest->tpl);
    memcpy(dest->input_buf[i], src->input_buf[i], sizeof(double) * dest->tpl);
  }
}



















