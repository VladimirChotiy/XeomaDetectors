#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_actionAbout_QT_triggered();
    void on_actionAboutProgramm_triggered();
    void on_actionClose_triggered();
};
#endif // MAINWINDOW_H
