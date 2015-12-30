/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Teunis van Beelen
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


#include "mainwindow.h"


// #define DEBUG_VIDEOPLAYER

#ifdef DEBUG_VIDEOPLAYER
  FILE *debug_vpr;
#endif


// Next parts of code are tested with VLC media player 2.1.2 and later with 2.1.5 Rincewind on Linux.
// On windows it's disabled because the console interface of VLC on windows is broken.
// Once they (videolan.org) has fixed this, we can test it and hopefully enable it on windows too.
void UI_Mainwindow::start_stop_video()
{
  if(video_player->status != VIDEO_STATUS_STOPPED)
  {
    stop_video_generic();

    return;
  }

  if(playback_realtime_active)
  {
    return;
  }

  if(live_stream_active)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open a video during a live stream.");
    messagewindow.exec();
    return;
  }

  if(video_player->status != VIDEO_STATUS_STOPPED)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There is already a video running.");
    messagewindow.exec();
    return;
  }

  if(signalcomps < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Put some signals on the screen first.");
    messagewindow.exec();
    return;
  }

  if(annot_editor_active)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Close the annotation editor first.");
    messagewindow.exec();
    return;
  }

  strcpy(videopath, QFileDialog::getOpenFileName(this, "Select video", QString::fromLocal8Bit(recent_opendir),
                                                 "Video files (*.ogv *.OGV *.ogg *.OGG *.mkv *.MKV *.avi *.AVI"
                                                 " *.mp4 *.MP4 *.mpeg *.MPEG *.mpg *.MPG *.wmv *.WMV)").toLocal8Bit().data());

  if(!strcmp(videopath, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, videopath, MAX_PATH_LENGTH);

  video_player->utc_starttime = parse_date_time_stamp(videopath);

  if(video_player->utc_starttime < 0LL)
  {
    QMessageBox messagewindow(QMessageBox::Warning, "Warning", "Unable to get startdate and starttime from video filename.\n"
                                                              " \nAssume video starttime equals EDF/BDF starttime?\n ");
    messagewindow.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    messagewindow.setDefaultButton(QMessageBox::Yes);
    if(messagewindow.exec() == QMessageBox::Cancel)  return;

    video_player->utc_starttime = edfheaderlist[sel_viewtime]->utc_starttime;
  }

  video_player->stop_det_counter = 0;

  video_player->fpos = 0;

  video_player->starttime_diff = (int)(edfheaderlist[sel_viewtime]->utc_starttime - video_player->utc_starttime);

  if((edfheaderlist[sel_viewtime]->utc_starttime + edfheaderlist[sel_viewtime]->recording_len_sec) < video_player->utc_starttime)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The video registration and the EDF/BDF registration do not overlap (in time)!");
    messagewindow.exec();
    return;
  }

  if((video_player->utc_starttime + 259200LL) < edfheaderlist[sel_viewtime]->utc_starttime)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The video registration and the EDF/BDF registration do not overlap (in time)!");
    messagewindow.exec();
    return;
  }

  video_process = new QProcess(this);

#ifdef Q_OS_WIN32
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert("PATH", env.value("PATH") + ";C:\\Program Files\\VideoLAN\\VLC");

  video_process->setProcessEnvironment(env);
#endif

  connect(video_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(video_process_error(QProcess::ProcessError)));

  QStringList arguments;
  arguments << "--video-on-top" << "-I" << "rc";

  video_process->start("vlc", arguments);

  if(video_process->waitForStarted(5000) == false)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Unable to start VLC mediaplayer.\n"
                                                              "Check your installation of VLC.\n"
                                                              "Also, check if VLC is present in the PATH evironment variable.");
    messagewindow.exec();
    return;
  }

#ifdef DEBUG_VIDEOPLAYER
  debug_vpr = fopen("debug_vpr.txt", "wb");
#endif

  video_player->status = VIDEO_STATUS_STARTUP_1;

  video_player->poll_timer = 100;

  video_player->cntdwn_timer = 5000;

  video_poll_timer->start(video_player->poll_timer);

  video_act->setText("Stop video");
}


void UI_Mainwindow::video_poll_timer_func()
{
  int i, err, len, vpos=0;

  char buf[4096];

  if(video_player->status == VIDEO_STATUS_STOPPED)  return;

  if(video_player->status != VIDEO_STATUS_PAUSED)
  {
    video_player->cntdwn_timer -= video_player->poll_timer;
  }

  if(video_player->cntdwn_timer <= 0)
  {
    stop_video_generic();

    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Videoplayer: time-out.");
    messagewindow.exec();
    return;
  }

  len = mpr_read(buf, 4095);
  if(len < 1)
  {
    video_poll_timer->start(video_player->poll_timer);

    return;
  }

  if(video_player->status == VIDEO_STATUS_STARTUP_1)
  {
    if(!strncmp(buf, "Command Line Interface initialized.", 35))
    {
      video_player->status = VIDEO_STATUS_STARTUP_2;
    }
  }

  if(video_player->status < VIDEO_STATUS_PLAYING)
  {
    if(!strncmp(buf, "> ", 2))
    {
      if(video_player->status == VIDEO_STATUS_STARTUP_2)
      {
        mpr_write("clear\n");

        video_process->waitForBytesWritten(1000);

        video_player->status = VIDEO_STATUS_STARTUP_3;

        video_player->cntdwn_timer = 5000;
      }
      else if(video_player->status == VIDEO_STATUS_STARTUP_3)
        {
          strcpy(buf, "add ");

          strcat(buf, videopath);

          strcat(buf, "\n");

          mpr_write(buf);

          video_process->waitForBytesWritten(1000);

          video_player->status = VIDEO_STATUS_STARTUP_4;

          video_player->cntdwn_timer = 5000;
        }
        else if(video_player->status == VIDEO_STATUS_STARTUP_4)
          {
            mpr_write("volume 255\n");

            video_process->waitForBytesWritten(1000);

            video_player->status = VIDEO_STATUS_PLAYING;

            video_pause_act->setText("Pause");

            video_pause_act->setToolTip("Pause video");

            video_player->cntdwn_timer = 5000;
          }
    }

    video_poll_timer->start(video_player->poll_timer);

    return;
  }

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    if(!strncmp(buf, "> ", 2))
    {
      if((len > 4) && (buf[len-1] == '\n'))
      {
        err = 0;

        for(i=2; i<(len-1); i++)
        {
          if((buf[i] < '0') || (buf[i] > '9'))

          err = 1;

          break;
        }

        if(!err)
        {
          vpos = atoi(buf + 2);

          if(video_player->fpos != vpos)
          {
            jump_to_time_millisec(video_player->utc_starttime - edfheaderlist[sel_viewtime]->utc_starttime + (vpos * 1000LL));

            video_player->fpos = vpos;

            video_player->stop_det_counter = 0;
          }

          video_player->cntdwn_timer = 5000;
        }
      }
      else if(buf[2] == '\r')
      {
        video_player->stop_det_counter += video_player->poll_timer;

        if(video_player->stop_det_counter > 1500)
        {
          stop_video_generic();

          QMessageBox messagewindow(QMessageBox::NoIcon, "Stopped", "  \nVideo has reached the end       \n");
          messagewindow.exec();

          return;
        }
      }
    }

    mpr_write("get_time\n");

    video_process->waitForBytesWritten(1000);
  }

  if(!strncmp(buf, "( state stopped )", 17))
  {
    stop_video_generic();

    return;
  }

  video_poll_timer->start(video_player->poll_timer);
}


void UI_Mainwindow::video_player_seek(int sec)
{
  char str[512];

  if((video_player->status != VIDEO_STATUS_PLAYING) && (video_player->status != VIDEO_STATUS_PAUSED))  return;

  sec += video_player->starttime_diff;

  if(sec < 0)  sec = 0;

  sprintf(str, "seek %i\n", sec);

  mpr_write(str);

  video_process->waitForBytesWritten(1000);

  video_player->cntdwn_timer = 5000;
}


void UI_Mainwindow::video_player_toggle_pause()
{
  if(video_player->status == VIDEO_STATUS_STOPPED)
  {
    start_stop_video();

    return;
  }

  if((video_player->status != VIDEO_STATUS_PLAYING) && (video_player->status != VIDEO_STATUS_PAUSED))
  {
    return;
  }

  mpr_write("pause\n");

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player->status = VIDEO_STATUS_PAUSED;

    video_pause_act->setText("Play");

    video_pause_act->setToolTip("Play video");

    video_player->cntdwn_timer = 5000;
  }
  else
  {
    video_player->status = VIDEO_STATUS_PLAYING;

    video_pause_act->setText("Pause");

    video_pause_act->setToolTip("Pause video");
  }
}


void UI_Mainwindow::stop_video_generic()
{
  video_poll_timer->stop();

  if(video_player->status == VIDEO_STATUS_STOPPED)  return;

  video_player->status = VIDEO_STATUS_STOPPED;

  disconnect(video_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(video_process_error(QProcess::ProcessError)));

  mpr_write("quit\n");

  video_process->waitForFinished(1000);

  video_process->kill();

  delete video_process;

  video_act->setText("Start video");

  video_pause_act->setText("Play");

#ifdef DEBUG_VIDEOPLAYER
  fclose(debug_vpr);
#endif
}


void UI_Mainwindow::video_process_error(QProcess::ProcessError err)
{
  char str[1024];

  if(video_player->status == VIDEO_STATUS_STOPPED)  return;

  stop_video_generic();

  strcpy(str, "The process that runs the mediaplayer reported an error:\n");

  if(err == QProcess::FailedToStart)
  {
    strcat(str, "\nFailed to start.");
  }

  if(err == QProcess::Crashed)
  {
    strcat(str, "\nCrashed.");
  }

  if(err == QProcess::Timedout)
  {
    strcat(str, "\nTimed out.");
  }

  if(err == QProcess::WriteError)
  {
    strcat(str, "\nWrite error.");
  }

  if(err == QProcess::ReadError)
  {
    strcat(str, "\nRead error.");
  }

  if(err == QProcess::UnknownError)
  {
    strcat(str, "\nUnknown error.");
  }

  QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
  messagewindow.exec();
}


inline void UI_Mainwindow::mpr_write(const char *cmd_str)
{
#ifdef DEBUG_VIDEOPLAYER
  fprintf(debug_vpr, "edfbr: %s", cmd_str);
#endif

  video_process->write(cmd_str);
}


inline int UI_Mainwindow::mpr_read(char *buf, int sz)
{
#ifdef DEBUG_VIDEOPLAYER
  int n;

  n = video_process->readLine(buf, sz);

  if(n > 0)
  {
    fprintf(debug_vpr, "vlc: %s ", buf);

    for(int i=0; i<n; i++)
    {
      fprintf(debug_vpr, " 0x%02X", buf[i]);
    }

    fprintf(debug_vpr, "\n");
  }

  return n;
#else
  return video_process->readLine(buf, sz);
#endif
}








