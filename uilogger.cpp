#include "uilogger.h"
#include <QTextStream>
#include <QDateTime>
#include "mainwindow.h"
#include <QString>
#include <iostream>


UiLogger::UiLogger(UI_Mainwindow *parent, QString destination_path, QPlainTextEdit *editor) : QObject(parent) {

    this->main_window = parent;
    this->m_editor = editor;
    this->destination_path = destination_path;
    this->log_id = 0;
    this->filename = "ui_log.txt";
    this->full_path = this->destination_path + this->filename;

    if (!destination_path.isEmpty()) {
        this->log_file = new QFile(this->full_path);
        this->log_file->open(QIODevice::WriteOnly | QIODevice::Text);
    }

    this->montage_dir_path = this->destination_path + "montages\\";
    if (!this->montage_dir.exists(montage_dir_path)) {
        montage_dir.mkpath(montage_dir_path);
    }

    std::cerr << "Constructed logger\n";

}

/**
 * @brief UiLogger::write
 * @param log_event
 */
void UiLogger::write(LogEvents log_event) {

    QString text = QDateTime::currentDateTime().toString("\"dd.MM.yyyy hh:mm:ss:zzz ");
    text += QString("id %1\": { Event: ").arg(this->log_id);

    if (log_event == LogEvents::FILE_OPENED) {
        this->save_montage();
        text += QString("\"FILE_OPENED\", \"data\": {\"graph_dimensions\": %1, \"graph_bbox\": %2, \"montage_file\": \"%3.mtg\"}").arg(get_graph_dimensions(), get_graph_coords(), QString::number(this->log_id));
    }
    else if (log_event == LogEvents::FILE_CLOSED) {
        this->save_montage();
        text += QString("\"FILE_CLOSED\", \"data\": {\"montage_file\": \"%1.mtg\"}").arg(this->log_id);
    }
    else if (log_event == LogEvents::MONTAGE_CHANGED) {
        this->save_montage();
        text += QString("\"MONTAGE_CHANGED\", \"data\": {\"montage_file\": \"%1.mtg\"}").arg(this->log_id);
    }
    else if (log_event == LogEvents::CHANNELS_CHANGED) {
        this->save_montage();
        text += QString("\"CHANNELS_CHANGED\", \"data\": {\"montage_file\": \"%1.mtg\"}").arg(this->log_id);
    }
    else if (log_event == LogEvents::FILTER_CHANGED) {
        this->save_montage();
        text += QString("\"FILTER_CHANGED\", \"data\": {\"montage_file\": \"%1.mtg\"}").arg(this->log_id);
    }
    else if (log_event == LogEvents::AMPLITUDE_CHANGED) {
        this->save_montage();
        text += QString("\"AMPLITUDE_CHANGED\", \"data\": {\"montage_file\": \"%1.mtg\"}").arg(this->log_id);
    }
    else if (log_event == LogEvents::TIMESCALE_CHANGED) {
        text += QString("\"TIMESCALE_CHANGED\", \"data\": {\"time_scale\": \"%1\"}").arg(main_window->pagetime_string);
    }
    else if (log_event == LogEvents::TIME_POSITION_CHANGED) {
        text += QString("\"TIME_POSITION_CHANGED\", \"data\": {\"time\": \"%1\"}").arg(main_window->viewtime_string);
    }
    else if (log_event == LogEvents::VERTICAL_CHANGED) {
        this->save_montage();
        text += QString("\"VERTICAL_CHANGED\", \"data\": {\"montage_file\": \"%1\"}").arg(this->log_id);
    }
    else if (log_event == LogEvents::ZOOM_CHANGED) {
        this->save_montage();
        text += QString("\"ZOOM_CHANGED\", \"data\": {\"time\": \"%1\", \"time_scale\": \"%2\", \"montage_file\": \"%3.mtg\"}").arg(QString(main_window->viewtime_string), QString(main_window->pagetime_string), QString::number(this->log_id));
    }
    else if (log_event == LogEvents::WINDOW_MOVED) {
        text += QString("\"WINDOW_MOVED\", \"data\": {\"graph_box\": %1}").arg(get_graph_coords());
    }
    else if (log_event == LogEvents::WINDOW_RESIZED) {
        text += QString("\"WINDOW_RESIZED\", \"data\": {\"graph_dimensions\": %1, \"graph_bbox\": %2}").arg(get_graph_dimensions(), get_graph_coords());
    }
    text = text + " }\n";

    QTextStream out(this->log_file);
    out.setCodec("UTF-8");

    if (this->log_file != 0) {
        out << text;
    }

    this->log_id++;
}


QString UiLogger::get_graph_coords() {

    ViewCurve *maincurve = main_window->maincurve;

    int main_curve_width = maincurve->width();
    int main_curve_height = maincurve->height();

    QString top_left_x = QString::number(maincurve->mapToGlobal(QPoint(0,0)).x());
    QString top_left_y = QString::number(maincurve->mapToGlobal(QPoint(0,0)).y());

    QString top_right_x = QString::number(maincurve->mapToGlobal(QPoint(main_curve_width, 0)).x());
    QString top_right_y = QString::number(maincurve->mapToGlobal(QPoint(main_curve_width, 0)).y());

    QString bottom_left_x = QString::number(maincurve->mapToGlobal(QPoint(0, main_curve_height)).x());
    QString bottom_left_y = QString::number(maincurve->mapToGlobal(QPoint(0, main_curve_height)).y());

    QString bottom_right_x = QString::number(maincurve->mapToGlobal(QPoint(main_curve_width, main_curve_height)).x());
    QString bottom_right_y = QString::number(maincurve->mapToGlobal(QPoint(main_curve_width, main_curve_height)).y());

    QString graph_box = QString("{\"top_left\": [%1,%2], \"top_right\": [%3,%4], \"bottom_left\": [%5,%6], \"bottom_right\": [%7,%8]}").arg(top_left_x, top_left_y, top_right_x, top_right_y, bottom_left_x, bottom_left_y, bottom_right_x, bottom_right_y);

    return graph_box;

}

QString UiLogger::get_graph_dimensions() {

    if (main_window->maincurve != 0) {
        ViewCurve *maincurve = main_window->maincurve;

        int main_curve_width = maincurve->width();
        int main_curve_height = maincurve->height();

        return QString("[%1 , %2]").arg(QString::number(main_curve_width), QString::number(main_curve_height)).simplified();
    }
    else return QString("[]");


}

QString UiLogger::calculate_baselines() {
    QString channel_y_positions = "[";
    int baseline;
    for (int i = 0; i < main_window->signalcomps; i++) {
      baseline = main_window->maincurve->height() / (main_window->signalcomps + 1);
      baseline *= (i + 1);
      channel_y_positions.append(main_window->maincurve->mapToGlobal(QPoint(0, baseline)).y());
      channel_y_positions.append(", ");
    }
    channel_y_positions.append("]");
    return channel_y_positions;
}

int UiLogger::save_montage() {

    int i, j, k, n;

    QString montage_number = QString::number(this->log_id);
    QString montage_filename = montage_number + ".mtg";
    QString montage_file_path = this->montage_dir_path + montage_filename;
    QFile *montage_file;
    if (montage_file_path.isEmpty())
        return 1;
    montage_file = new QFile(montage_file_path);
    montage_file->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(montage_file);
    out.setCodec("UTF-8");
    QString text;

    if (main_window->files_open == 1)
        n = 0;
    else
        return 1;

    text += QString("<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_montage>\n");

    for (i = 0; i < main_window->signalcomps; i++) {

        if (!strcmp(main_window->signalcomp[i]->edfhdr->filename, main_window->edfheaderlist[n]->filename))
        {
            text += QString("  <signalcomposition>\n");

            text += QString("    <alias>%1</alias>\n").arg(main_window->signalcomp[i]->alias);
            text += QString("    <num_of_signals>%1</num_of_signals>\n").arg(main_window->signalcomp[i]->num_of_signals);
            text += QString("    <voltpercm>%1</voltpercm>\n").arg(main_window->signalcomp[i]->voltpercm);
            text += QString("    <screen_offset>%1</screen_offset>\n").arg(main_window->signalcomp[i]->screen_offset);
            text += QString("    <polarity>%1</polarity>\n").arg(main_window->signalcomp[i]->polarity);
            text += QString("    <color>%1</color>\n").arg(main_window->signalcomp[i]->color);

            if (main_window->signalcomp[i]->spike_filter) {
                text += QString("    <spike_filter_cnt>%1</spike_filter_cnt>\n").arg(1);
            }
            else {
                text += QString("    <spike_filter_cnt>%1</spike_filter_cnt>\n").arg(0);
            }

            text += QString("    <filter_cnt>%1</filter_cnt>\n").arg(main_window->signalcomp[i]->filter_cnt);
            text += QString("    <fidfilter_cnt>%1</fidfilter_cnt>\n").arg(main_window->signalcomp[i]->fidfilter_cnt);
            text += QString("    <ravg_filter_cnt>%1</ravg_filter_cnt>\n").arg(main_window->signalcomp[i]->ravg_filter_cnt);

            for (j = 0; j < main_window->signalcomp[i]->num_of_signals; j++) {
                text += QString("    <signal>\n");
                text += QString("      <label>%1</label>\n").arg(main_window->signalcomp[i]->edfhdr->edfparam[main_window->signalcomp[i]->edfsignal[j]].label);
                text += QString("      <factor>%1</factor>\n").arg(main_window->signalcomp[i]->factor[j]);
                text += QString("    </signal>\n");
            }

            if (main_window->signalcomp[i]->spike_filter) {
                text += QString("    <spike_filter>\n");
                text += QString("      <velocity>%1</velocity>\n").arg(main_window->signalcomp[i]->spike_filter_velocity);
                text += QString("      <holdoff>%1</holdoff>\n").arg(main_window->signalcomp[i]->spike_filter_holdoff);
                text += QString("    </spike_filter>\n");
            }

            for (j = 0; j < main_window->signalcomp[i]->filter_cnt; j++) {
                text += QString("    <filter>\n");
                text += QString("      <LPF>%1</LPF>\n").arg(main_window->signalcomp[i]->filter[j]->is_LPF);
                text += QString("      <frequency>%1</frequency>\n").arg(main_window->signalcomp[i]->filter[j]->cutoff_frequency);
                text += QString("    </filter>\n");
            }

            for (j = 0; j < main_window->signalcomp[i]->ravg_filter_cnt; j++) {
                text += QString("    <ravg_filter>\n");
                text += QString("      <type>%1</type>\n").arg(main_window->signalcomp[i]->ravg_filter[j]->type);
                text += QString("      <size>%1</size>\n").arg(main_window->signalcomp[i]->ravg_filter[j]->size);
                text += QString("    </ravg_filter>\n");
            }

            for (j = 0; j < main_window->signalcomp[i]->fidfilter_cnt; j++) {
                text += QString("    <fidfilter>\n");
                text += QString("      <type>%1</type>\n").arg(main_window->signalcomp[i]->fidfilter_type[j]);
                text += QString("      <frequency>%1</frequency>\n").arg(main_window->signalcomp[i]->fidfilter_freq[j]);
                text += QString("      <frequency2>%1</frequency2>\n").arg(main_window->signalcomp[i]->fidfilter_freq2[j]);
                text += QString("      <ripple>%1</ripple>\n").arg(main_window->signalcomp[i]->fidfilter_ripple[j]);
                text += QString("      <order>%1</order>\n").arg(main_window->signalcomp[i]->fidfilter_order[j]);
                text += QString("      <model>%1</model>\n").arg(main_window->signalcomp[i]->fidfilter_model[j]);
                text += QString("    </fidfilter>\n");
            }

            if (main_window->signalcomp[i]->fir_filter != NULL) {
                k = fir_filter_size(main_window->signalcomp[i]->fir_filter);
                text += QString("    <fir_filter>\n");
                text += QString("      <size>%1</size>\n").arg(k);
                for(j = 0; j < k; j++) {
                    text += QString("      <tap>%1</tap>\n").arg(fir_filter_tap(j, main_window->signalcomp[i]->fir_filter));
                }
                text += QString("    </fir_filter>\n");
            }

            if (main_window->signalcomp[i]->plif_ecg_filter != NULL) {
                text += QString("    <plif_ecg_filter>\n");
                text += QString("      <plf>%1</plf>\n").arg(main_window->signalcomp[i]->plif_ecg_subtract_filter_plf);
                text += QString("    </plif_ecg_filter>\n");
            }

            if (main_window->signalcomp[i]->ecg_filter != NULL) {
                text += QString("    <ecg_filter>\n");
                text += QString("      <type>1</type>\n");
                text += QString("    </ecg_filter>\n");
            }

            if (main_window->signalcomp[i]->zratio_filter != NULL) {
                text += QString("    <zratio_filter>\n");
                text += QString("      <type>1</type>\n");
                text += QString("      <crossoverfreq>%1</crossoverfreq>\n").arg(main_window->signalcomp[i]->zratio_crossoverfreq);
                text += QString("    </zratio_filter>\n");
            }

            if (main_window->signalcomp[i]->hasruler) {
                text += QString("    <floating_ruler>\n");
                text += QString("      <hasruler>1</hasruler>\n");
                text += QString("      <ruler_x_position>%1</ruler_x_position>\n").arg(main_window->maincurve->ruler_x_position);
                text += QString("      <ruler_y_position>%1</ruler_y_position>\n").arg(main_window->maincurve->ruler_y_position);
                text += QString("      <floating_ruler_value>%1</floating_ruler_value>\n").arg(main_window->maincurve->floating_ruler_value);
                text += QString("    </floating_ruler>\n");
            }
            text += QString("  </signalcomposition>\n");
        }
    }

    text += QString("  <pagetime>%1</pagetime>\n").arg(main_window->pagetime);

    struct spectrumdocksettings settings;

    for (i = 0; i < MAXSPECTRUMDOCKS; i++) {
        if (main_window->spectrumdock[i]->dock->isVisible()) {
            main_window->spectrumdock[i]->getsettings(&settings);
            if (settings.signalnr >= 0) {
                text += QString("  <powerspectrumdock>\n");
                text += QString("    <signalnum>%1</signalnum>\n").arg(settings.signalnr);
                text += QString("    <amp>%1</amp>\n").arg(settings.amp);
                text += QString("    <log_min_sl>%1</log_min_sl>\n").arg(settings.log_min_sl);
                text += QString("    <wheel>%1</wheel>\n").arg(settings.wheel);
                text += QString("    <span>%1</span>\n").arg(settings.span);
                text += QString("    <center>%1</center>\n").arg(settings.center);
                text += QString("    <log>%1</log>\n").arg(settings.log);
                text += QString("    <sqrt>%1</sqrt>\n").arg(settings.sqrt);
                text += QString("    <colorbar>%1</colorbar>\n").arg(settings.colorbar);
                text += QString("    <maxvalue>%1</maxvalue>\n").arg(settings.maxvalue);
                text += QString("    <maxvalue_sqrt>%1</maxvalue_sqrt>\n").arg(settings.maxvalue_sqrt);
                text += QString("    <maxvalue_vlog>%1</maxvalue_vlog>\n").arg(settings.maxvalue_vlog);
                text += QString("    <maxvalue_sqrt_vlog>%1</maxvalue_sqrt_vlog>\n").arg(settings.maxvalue_sqrt_vlog);
                text += QString("    <minvalue_vlog>%1</minvalue_vlog>\n").arg(settings.minvalue_vlog);
                text += QString("    <minvalue_sqrt_vlog>%1</minvalue_sqrt_vlog>\n").arg(settings.minvalue_sqrt_vlog);
                text += QString("  </powerspectrumdock>\n");
            }
        }
    }

    text += QString("</" PROGRAM_NAME "_montage>\n");

    out << text;
    montage_file->close();

    if (main_window->files_open == 1) {
        strlcpy(&main_window->recent_file_mtg_path[0][0], montage_file_path.toStdString().c_str(), MAX_PATH_LENGTH);
    }

    return 0;

}

/**
 * @brief UiLogger::set_destination_path
 * @param destination: Path where to create the the log file
 * @details initialise a destination directory and log file
 */
void UiLogger::set_destination_path(QString destination) {
    this->destination_path = destination;
    this->filename = "ui_log.txt";
    this->full_path = this->destination_path + this->filename;

    if (!destination_path.isEmpty()) {
        this->log_file = new QFile(this->full_path);
        this->log_file->open(QIODevice::WriteOnly | QIODevice::Text);
    }

    this->montage_dir_path = this->destination_path + "montages\\";
    if (!this->montage_dir.exists(montage_dir_path)) {
        montage_dir.mkpath(montage_dir_path);
    }
}


UiLogger::~UiLogger() {
    if (this->log_file != 0) {
        this->log_file->close();
    }
}
