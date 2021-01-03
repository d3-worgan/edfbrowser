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
    explicit UiLogger(UI_Mainwindow *main_window, QString destination_directory_path, QPlainTextEdit *editor = 0);
    UiLogger(QString destination_directory_path);
    ~UiLogger();

    void set_destination_path(QString destination);

    bool writing_log;
    bool save_screenshots;
    QString current_edf_path;

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

    QDir desination_directory;
    QString destination_directory_path;
    QString log_file_name;
    QString log_full_path;
    QFile *log_file;
    QPlainTextEdit *m_editor;


    QDir montage_dir;
    QString montage_dir_name;
    QString montage_dir_path;


    QDir screenshot_directory;
    QString screenshot_directory_location;



public slots:
    void write(LogEvents log_event);
    QString get_graph_coords();
    QString get_graph_dimensions();
    int save_montage();
    void save_screenshot();
    QString calculate_baselines();

};

#endif // UILOGGER_H

