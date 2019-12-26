#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Request.h"
#include <deque>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     void ShowProcessedReqTable();
     void ShowFailedReqTable(class TimeManager* time_manager);

     void DrawGraphicGraphicReqPerFix(TimeManager* time_manager);
     void DrawGraphicGraphicPPerFix(TimeManager* time_manager);

private slots:
    void on_pushButton_clicked();

private:
    void AddRowToTableFromDeq(class QStandardItemModel* model, std::deque<Request>* deq);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
