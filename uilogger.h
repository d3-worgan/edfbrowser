#ifndef UILOGGER_H
#define UILOGGER_H

#include <QString>
#include <QFile>
#include <QPlainTextEdit>
#include <QObject>
#include "mainwindow.h"


class UiLogger: public QObject {
    Q_OBJECT
public:
    explicit UiLogger(UI_Mainwindow *main_window, QString destination_path, QPlainTextEdit *editor = 0);
    UiLogger(QString destination_path);
    ~UiLogger();

    void set_destination_path(QString destination);

    enum class LogEvents {
        FILE_OPENED,
        FILE_CLOSED,
        MONTAGE_CHANGED,
        CHANNELS_CHANGED,
        FILTER_CHANGED,
        AMPLITUDE_CHANGED,
        TIMESCALE_CHANGED,
        TIME_POSITION_CHANGED,
        VERTICAL_CHANGED,
        ZOOM_CHANGED,
        WINDOW_MOVED,
        WINDOW_RESIZED
    };

private:
    UI_Mainwindow *main_window;
    int log_id;
    QString destination_path;
    QString filename;
    QString full_path;
    QFile *log_file;
    QPlainTextEdit *m_editor;
    QDir montage_dir;
    QString montage_dir_name;
    QString montage_dir_path;

public slots:
    void write(LogEvents log_event);
    QString get_graph_coords();
    QString get_graph_dimensions();
    int save_montage();
    QString calculate_baselines();

};

#endif // UILOGGER_H

