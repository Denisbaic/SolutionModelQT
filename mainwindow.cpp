#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TimeManager.h"
#include "QStandardItemModel"
#include "QStandardItem"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ShowProcessedReqTable(){
    QStandardItemModel *model = new QStandardItemModel;

    //Заголовки столбцов
    QStringList horizontalHeader;
    horizontalHeader.append("Время на обработку заявки");
    horizontalHeader.append("Время входа в систему");
    horizontalHeader.append("Время начала работы над заявкой");
    horizontalHeader.append("Время выхода");

    //Заголовки строк
    QStringList verticalHeader;

    model->setHorizontalHeaderLabels(horizontalHeader);
    model->setVerticalHeaderLabels(verticalHeader);

    while(!Worker::ProcessedReqQueue.empty()){
        AddRowToTableFromDeq(model,&Worker::ProcessedReqQueue);
    }

    ui->tableProccessedRequests->setModel(model);

    ui->tableProccessedRequests->resizeRowsToContents();
    ui->tableProccessedRequests->resizeColumnsToContents();
}

void MainWindow::ShowFailedReqTable(TimeManager* time_manager)
{
    QStandardItemModel *model = new QStandardItemModel;

    //Заголовки столбцов
    QStringList horizontalHeader;
    horizontalHeader.append("Время на обработку заявки");
    horizontalHeader.append("Время входа в систему");
    horizontalHeader.append("Время начала работы над заявкой");
    horizontalHeader.append("Время выхода");

    //Заголовки строк
    QStringList verticalHeader;

    model->setHorizontalHeaderLabels(horizontalHeader);
    model->setVerticalHeaderLabels(verticalHeader);

    while(!time_manager->ReqFailed.empty()){
        AddRowToTableFromDeq(model,&time_manager->ReqFailed);
    }

    ui->tableFailedRequests->setModel(model);

    ui->tableFailedRequests->resizeRowsToContents();
    ui->tableFailedRequests->resizeColumnsToContents();
}

void MainWindow::DrawGraphic(TimeManager* time_manager)
{
    ui->widget->addGraph();
    ui->widget->graph(0)->setData(time_manager->Time,time_manager->ReqCountPerFix);

    ui->widget->xAxis->setLabel("Время");
    ui->widget->yAxis->setLabel("Кол-во заявок");

    ui->widget->xAxis->setRange(0, time_manager->CurrentTime);
    ui->widget->yAxis->setRange(-1, time_manager->Limit+1);
    ui->widget->replot();
}

void MainWindow::on_pushButton_clicked()
{
    int ReqNeed=ui->TBReqNeed->toPlainText().toInt();
    double AverageAdmissionTime=ui->TBAdmissionTime->toPlainText().toDouble(),
        AverageServiceTime=ui->TBAverageServiceTime->toPlainText().toDouble(),
        ReqLimit=ui->TBReqLimit->toPlainText().toDouble();
    TimeManager time_manager(ReqNeed, 1.0/AverageServiceTime, 1.0/AverageAdmissionTime);
    time_manager.Limit=ReqLimit;
    //AverageServiceTime =0.04 AverageReqAdmissionTime=0.1
    time_manager.AddNextReqBeforeSomeTime();

        while (time_manager.ReqNeed>time_manager.ReqAdded || !time_manager.TimeHandle.empty())
        {
            Event event_=time_manager.MoveTime();
            if(event_.event_destination==EventDestination::NewReqest)
            {
                if(time_manager.ReqNeed != time_manager.ReqAdded)
                    time_manager.AddNextReqBeforeSomeTime();
                time_manager.RequestInWork();
            }
            else if (event_.event_destination==EventDestination::SomeRequestTimeEnd)
            {
                event_.workerPtr->ReqAcceptedCount++;
                event_.req.ExitTime = time_manager.CurrentTime;
                event_.workerPtr->ReqPriority = -1;

                Worker::ProcessedReqQueue.push_back(event_.req);
                time_manager.CheckReqContainer();
            }
        }


        ui->TBReqAccessed1->setText(QString::number(time_manager.w1.ReqAcceptedCount));
        ui->TBReqAccessed2->setText(QString::number(time_manager.w2.ReqAcceptedCount));
        ui->TBReqAccessed3->setText(QString::number(time_manager.w3.ReqAcceptedCount));
        ui->TBReqAccessed4->setText(QString::number(time_manager.w4.ReqAcceptedCount));
        ui->TBReqAccessed5->setText(QString::number(time_manager.w5.ReqAcceptedCount));
        ui->TBReqFailedCoint->setText(QString::number(time_manager.ReqFailed.size()));

        ui->TBP ->setText(QString::number(time_manager.GetSystemUtilizationP()));
        ui->TBTq->setText(QString::number(time_manager.GetAverageWaitingTimeForAnApplicationInQueueTq()));
        ui->TBTs->setText(QString::number(time_manager.GetAverageTimeSpentByTheApplicationInTheSystemTs()));
        ui->TBNq->setText(QString::number(time_manager.GetTimeAverageNumberOfRequestsInTheQueueNq()));
        ui->TBNs->setText(QString::number(time_manager.GetTimeAverageNumberOfRequirementsInTheSystemNs()));
        ui->TBCa->setText(QString::number(time_manager.GetAbsoluteSystemCapacityCa()));

        DrawGraphic(&time_manager);

        ShowProcessedReqTable();
        ShowFailedReqTable(&time_manager);
    }
void MainWindow::AddRowToTableFromDeq(QStandardItemModel *model, std::deque<Request> *deq)
{
    if(deq->empty())
        return;
    auto TempReq=*deq->begin();
    deq->pop_front();

    int Index=model->rowCount();
    //WorkTime
    QStandardItem* item = new QStandardItem(QString::number(TempReq.WorkTime));
    model->setItem(Index, 0, item);

    //ArrivalTime
    item = new QStandardItem(QString::number(TempReq.ArrivalTime));
    model->setItem(Index, 1, item);

    //WorkBeginTime
    item = new QStandardItem(QString::number(TempReq.WorkBeginTime));
    model->setItem(Index, 2, item);

    //ExitTime
    item = new QStandardItem(QString::number(TempReq.ExitTime));
    model->setItem(Index, 3, item);
}
