/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017 Teunis van Beelen
*
* Email: teuniz@gmail.com
*
***************************************************************************
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
*/



#include "edf_annot_list_ii.h"


#define EDF_ANNOT_MEMBLOCKSZ_II   1000


int edfplus_annotation_ii_add_item(struct annotation_list_ii *list, struct annotationblock_ii annotation)
{
  struct annotationblock_ii *tmp_block_list;

  if(list == NULL)  return -1;

  if(list->sz >= list->mem_sz)
  {
    tmp_block_list = (struct annotationblock_ii *)realloc(list->items, (list->mem_sz + EDF_ANNOT_MEMBLOCKSZ_II) * sizeof(struct annotationblock_ii));
    if(tmp_block_list == NULL)
    {
      return -2;
    }

    list->items = tmp_block_list;

    list->mem_sz += EDF_ANNOT_MEMBLOCKSZ_II;
  }

  list->items[list->sz++] = annotation;

  return 0;
}


int edfplus_annotation_ii_size(struct annotation_list_ii *list)
{
  if(list == NULL)  return 0;

  return(list->sz);
}


void edfplus_annotation_ii_empty_list(struct annotation_list_ii *list)
{
  if(list == NULL)  return;

  free(list->items);
  list->items = NULL;
  list->sz = 0;
  list->mem_sz = 0;
}


void edfplus_annotation_ii_remove(struct annotation_list_ii *list, int n)
{
  int i;

  if(list == NULL)  return;

  if((n >= list->sz) || (n < 0))  return;

  for(i=n; i<(list->sz - 1); i++)
  {
    list->items[i] = list->items[i+1];
  }

  list->sz--;
}


int edfplus_annotation_ii_get_item(struct annotation_list_ii *list, struct annotationblock_ii *annot, int n)
{
  if(list == NULL)  return -1;

  if(annot == NULL)  return -2;

  if((n >= list->sz) || (n < 0))  return -3;

  *annot = list->items[n];

  return 0;
}


void edfplus_annotation_ii_sort(struct annotation_list_ii *list)  /* sort the annotationlist based on the onset time */
{
  int i, j;

  struct annotationblock_ii tmp_annot;

  if(list == NULL)  return;

  for(i=0; i<(list->sz - 1); i++)
  {
    for(j=i+1; j>0; j--)
    {
      if(list->items[j-1].onset <= list->items[j].onset)
      {
        break;
      }

      tmp_annot = list->items[j-1];

      list->items[j-1] = list->items[j];

      list->items[j] = tmp_annot;
    }
  }
}


struct annotation_list_ii * edfplus_annotation_ii_copy_list(struct annotation_list_ii *list)
{
  struct annotation_list_ii *cpy;

  if(list == NULL)  return NULL;

  cpy = (struct annotation_list_ii *)calloc(1, sizeof(struct annotation_list_ii));
  if(cpy == NULL)
  {
    return NULL;
  }

  if(list->sz > 0)
  {
    cpy->items = (struct annotationblock_ii *)calloc(1, sizeof(struct annotationblock_ii) * list->mem_sz);
    if(cpy->items == NULL)
    {
      free(cpy);
      return NULL;
    }

    cpy->mem_sz = list->mem_sz;

    cpy->sz = list->sz;

    memcpy(cpy->items, list->items, cpy->sz * sizeof(struct annotationblock_ii));
  }

  return cpy;
}


// int edfplus_annotation_ii_get_tal_timestamp_digit_cnt(struct edfhdrblock *hdr)
// {
//   int timestamp_digits;
//
//   char scratchpad[256];
//
//   long long time;
//
//
//   if(hdr==NULL)
//   {
//     return(-1);
//   }
//
//   time = (hdr->datarecords * hdr->long_data_record_duration) / TIME_DIMENSION;
//
// #ifdef _WIN32
//   timestamp_digits = __mingw_snprintf(scratchpad, 256, "%lli", time);
// #else
//   timestamp_digits = snprintf(scratchpad, 256, "%lli", time);
// #endif
//
//   return(timestamp_digits);
// }
//
//
// int edfplus_annotation_ii_get_tal_timestamp_decimal_cnt(struct edfhdrblock *hdr)
// {
//   int i, j,
//       timestamp_decimals;
//
//
//   if(hdr==NULL)
//   {
//     return(-1);
//   }
//
//   j = 10;
//
//   for(timestamp_decimals=7; timestamp_decimals>0; timestamp_decimals--)
//   {
//     if(hdr->long_data_record_duration % j)
//     {
//       break;
//     }
//
//     j *= 10;
//   }
//
//   if((hdr->edfplus==1)||(hdr->bdfplus==1))
//   {
//     j = 10;
//
//     for(i=7; i>0; i--)
//     {
//       if(hdr->starttime_offset % j)
//       {
//         break;
//       }
//
//       j *= 10;
//     }
//
//     if(i > timestamp_decimals)
//     {
//       timestamp_decimals = i;
//     }
//   }
//
//   return(timestamp_decimals);
// }
//
//
// int edfplus_annotation_ii_get_max_annotation_strlen(struct annotation_list_ii **list)
// {
//   int j,
//       len,
//       annot_descr_len=0,
//       annot_duration_len=0,
//       timestamp_digits=0,
//       timestamp_decimals=0;
//
//   char scratchpad[256];
//
//   struct annotation_list_ii *annot;
//
//
//   annot = list[0];
//
//   if(annot==NULL)
//   {
//     return(0);
//   }
//
//   while(annot!=NULL)
//   {
// #ifdef _WIN32
//     len = __mingw_snprintf(scratchpad, 256, "%lli", annot->onset / TIME_DIMENSION);
// #else
//     len = snprintf(scratchpad, 256, "%lli", annot->onset / TIME_DIMENSION);
// #endif
//
//     if(len > timestamp_digits)
//     {
//       timestamp_digits = len;
//     }
//
//     j = 10;
//
//     for(len=7; len>0; len--)
//     {
//       if(annot->onset % j)
//       {
//         break;
//       }
//
//       j *= 10;
//     }
//
//     if(len > timestamp_decimals)
//     {
//       timestamp_decimals = len;
//     }
//
//     len = strlen(annot->annotation);
//
//     if(len>annot_descr_len)
//     {
//       annot_descr_len = len;
//     }
//
//     len = strlen(annot->duration);
//
//     if(len>annot_duration_len)
//     {
//       annot_duration_len = len;
//     }
//
//     annot = annot->next_annotation;
//   }
//
//   if(annot_duration_len)
//   {
//     annot_duration_len++;
//   }
//
//   if(timestamp_decimals)
//   {
//     timestamp_decimals++;
//   }
//
// // printf("annot_descr_len is %i\n"
// //        "annot_duration_len is %i\n"
// //        "timestamp_digits is %i\n"
// //        "timestamp_decimals is %i\n",
// //        annot_descr_len,
// //        annot_duration_len,
// //        timestamp_digits,
// //        timestamp_decimals);
//
//   return(annot_descr_len + annot_duration_len + timestamp_digits + timestamp_decimals + 4);
// }

















