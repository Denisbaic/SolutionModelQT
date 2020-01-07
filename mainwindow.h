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

     void ShowWorkerInfoTable(class TimeManager* time_manager);
     void ShowProcessedReqTable();
     void ShowFailedReqTable(TimeManager* time_manager);

     void DrawGraphicNs(TimeManager* time_manager);
     void DrawGraphicNq(TimeManager* time_manager);
     void DrawGraphicReqInDeqPerFix(TimeManager* time_manager);
     void DrawGraphicReqInSystemPerFix(TimeManager* time_manager);
     void DrawGraphicPPerFix(TimeManager* time_manager);

     void DrawGraphicExpAdmission(TimeManager* time_manager);
     void DrawGraphicExpService(TimeManager* time_manager);

     void DrawGraphicExpDensityService(TimeManager* time_manager);
     void DrawGraphicExpDensityAdmission(TimeManager* time_manager);
private slots:
    void on_pushButton_clicked();

private:

    void AddRowToTableFromDeq(class QStandardItemModel* model, std::deque<Request>* deq);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
