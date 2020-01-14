#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TimeManager.h"
#include "QStandardItemModel"
#include "QStandardItem"
#include <iostream>
#include <QLCDNumber>
#include "qcustomplot.h"
#include <algorithm>
#include <QList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix("E:/QTProjects/SolutionModelAlexey/ShemeSMO.png");
     QPixmap pix2("E:/QTProjects/SolutionModelAlexey/Block-sheme SMO.png");
    ui->label_18->setPixmap(pix);
    ui->label_19->setPixmap(pix2);

    QStandardItemModel *model = new QStandardItemModel;
    //Заголовки столбцов
    QStringList horizontalHeader;
    horizontalHeader.append("Приоритет");
    horizontalHeader.append("Вероятность");
    model->setHorizontalHeaderLabels(horizontalHeader);

    ui->TLowPriority->setModel(model);

    model = new QStandardItemModel;


    model->setHorizontalHeaderLabels(horizontalHeader);



    ui->THighPriority->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetPriorityFromTables(TimeManager *time_manager)
{
    time_manager->LowPriorityArr.empty();
    time_manager->HighPriorityArr.empty();

    QVector<int> keys;
    QList<double> priority={0};


    for (int i=0;i< ui->TLowPriority->model()->rowCount();++i) {
      keys.append(ui->TLowPriority->model()->index(i,0).data().toInt());
    }
    priority.append(ui->TLowPriority->model()->index(0,1).data().toDouble());
    for (int i=1;i< ui->TLowPriority->model()->rowCount();++i) {
      priority.append(ui->TLowPriority->model()->index(i,1).data().toDouble());
    }
    for (int i=1;i<= ui->TLowPriority->model()->rowCount();++i) {
      priority[i]+=priority[i-1];
    }
    for (int i=0;i<keys.size();++i) {
        time_manager->LowPriorityArr.insert(keys[i],QPair<double,double>(priority[i],priority[i+1]));
    }
    keys={};
    priority={0};

    for (int i=0;i< ui->THighPriority->model()->rowCount();++i) {
      keys.append(ui->THighPriority->model()->index(i,0).data().toInt());
    }
    priority.append(ui->THighPriority->model()->index(0,1).data().toDouble());
    for (int i=1;i< ui->THighPriority->model()->rowCount();++i) {
      priority.append(ui->THighPriority->model()->index(i,1).data().toDouble());
    }
    for (int i=1;i<= ui->THighPriority->model()->rowCount();++i) {
      priority[i]+=priority[i-1];
    }
    for (int i=0;i<keys.size();++i) {
        time_manager->HighPriorityArr.insert(keys[i],QPair<double,double>(priority[i],priority[i+1]));
    }

}

void MainWindow::ShowWorkerInfoTable(TimeManager *time_manager)
{
    QStandardItemModel *model = new QStandardItemModel;
    //Заголовки столбцов
    QStringList horizontalHeader;
    horizontalHeader.append("Время в работе");
    horizontalHeader.append("Заявок принято");

    //Заголовки строк
    QStringList verticalHeader;

    model->setHorizontalHeaderLabels(horizontalHeader);
    model->setVerticalHeaderLabels(verticalHeader);

    for (int i=0;i<time_manager->CountOfWorkers;++i) {
        int Index=model->rowCount();
        QStandardItem* item = new QStandardItem(QString::number(time_manager->GroupOfWorkers[i].TimeInWork));
        model->setItem(Index, 0, item);
        item = new QStandardItem(QString::number(time_manager->GroupOfWorkers[i].ReqAcceptedCount));
        model->setItem(Index, 1, item);
    }
    ui->TWorkerInfo->setModel(model);

    ui->TWorkerInfo->resizeRowsToContents();
    ui->TWorkerInfo->resizeColumnsToContents();
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

void MainWindow::DrawRequestBars(TimeManager *time_manager)
{
    QCustomPlot *customPlot = ui->BarsRequests;

    customPlot->clearGraphs();
    customPlot->clearItems();
    customPlot->clearPlottables();
    // create empty bar chart objects:

    QCPBars *nuclear = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    QCPBars *fossil = new QCPBars(customPlot->xAxis, customPlot->yAxis);

    nuclear->setAntialiased(false);
    fossil->setAntialiased(false);

    nuclear->setStackingGap(1);
    fossil->setStackingGap(1);
    // set names and colors:
    fossil->setName("Заявки с абсолютным приоритетом");
    fossil->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    fossil->setBrush(QColor(111, 9, 176));
    nuclear->setName("Заявки с относительным приоритетом");
    nuclear->setPen(QPen(QColor(250, 170, 20).lighter(150)));
    nuclear->setBrush(QColor(250, 170, 20));

    // stack bars on top of each other:
    nuclear->moveAbove(fossil);


    QSet<int> _tiks;
    for(auto& elem : time_manager->HighPriorityArr.keys()){
        _tiks.insert(elem);
    }
    for(auto& elem : time_manager->LowPriorityArr.keys()){
        _tiks.insert(elem);
    }
    // prepare x axis with country labels:

    QVector<double> ticks;
    QVector<QString> labels;
    for(auto elem: _tiks){
        ticks.append(elem);
        labels.append(QString::number(elem));

    }
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);


    // Add data:

    QSet<int>::iterator max = std::max_element(_tiks.begin(), _tiks.end());
    QVector<double> fossilData(*max), nuclearData(*max);

    for(int i=0; i< Worker::ProcessedReqQueue.size(); ++i){
        if(Worker::ProcessedReqQueue[i].isReqAbsolute){
            fossilData[Worker::ProcessedReqQueue[i].ReqPriority-1]++;
        }else{
            nuclearData[Worker::ProcessedReqQueue[i].ReqPriority-1]++;
        }
    }

    qSort(ticks);
    fossil->setData(ticks, fossilData);
    nuclear->setData(ticks, nuclearData);

    // setup legend:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    customPlot->legend->setBrush(QColor(255, 255, 255, 100));
    customPlot->legend->setBorderPen(Qt::NoPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    customPlot->legend->setFont(legendFont);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    customPlot->xAxis->setRange(0, *max+1);
    customPlot->yAxis->setRange(0, time_manager->ReqNeed);
    customPlot->replot();
}

void MainWindow::DrawGraphicNs(TimeManager *time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->NsPerFix.begin(), time_manager->NsPerFix.end());
    ui->GraphicNs->setInteraction(QCP::iRangeDrag);
    ui->GraphicNs->setInteraction(QCP::iRangeZoom);

    ui->GraphicNs->clearGraphs();
    ui->GraphicNs->addGraph();

    ui->GraphicNs->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicNs->graph(0)->setData(time_manager->Time,time_manager->NsPerFix);

    ui->GraphicNs->xAxis->setLabel("Время");
    ui->GraphicNs->yAxis->setLabel("Кол-во заявок");

    ui->GraphicNs->xAxis->setRange(0, time_manager->CurrentTime);
    ui->GraphicNs->yAxis->setRange(0, *max);
    ui->GraphicNs->replot();
}

void MainWindow::DrawGraphicNq(TimeManager *time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->NqPerFix.begin(), time_manager->NqPerFix.end());
    ui->GraphicNq->setInteraction(QCP::iRangeDrag);
    ui->GraphicNq->setInteraction(QCP::iRangeZoom);

    ui->GraphicNq->clearGraphs();
    ui->GraphicNq->addGraph();
    ui->GraphicNq->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicNq->graph(0)->setData(time_manager->Time,time_manager->NqPerFix);

    ui->GraphicNq->xAxis->setLabel("Время");
    ui->GraphicNq->yAxis->setLabel("Кол-во заявок");

    ui->GraphicNq->xAxis->setRange(0, time_manager->CurrentTime);
    ui->GraphicNq->yAxis->setRange(0, *max);
    ui->GraphicNq->replot();
}

void MainWindow::DrawGraphicReqInDeqPerFix(TimeManager* time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->ReqCountPerFix.begin(), time_manager->ReqCountPerFix.end());
    ui->GraphicReqPerFix->setInteraction(QCP::iRangeDrag);
    ui->GraphicReqPerFix->setInteraction(QCP::iRangeZoom);

    ui->GraphicReqPerFix->clearGraphs();
    ui->GraphicReqPerFix->addGraph();
    ui->GraphicReqPerFix->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicReqPerFix->graph(0)->setData(time_manager->Time,time_manager->ReqCountPerFix);

    ui->GraphicReqPerFix->xAxis->setLabel("Время");
    ui->GraphicReqPerFix->yAxis->setLabel("Кол-во заявок");

    ui->GraphicReqPerFix->xAxis->setRange(0, time_manager->CurrentTime);
    ui->GraphicReqPerFix->yAxis->setRange(0, *max);
    ui->GraphicReqPerFix->replot();
}

void MainWindow::DrawGraphicReqInSystemPerFix(TimeManager *time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->ReqCountInSystemPerFix.begin(), time_manager->ReqCountInSystemPerFix.end());
    ui->GraphicReqInSystemPerFix->setInteraction(QCP::iRangeDrag);
    ui->GraphicReqInSystemPerFix->setInteraction(QCP::iRangeZoom);

    ui->GraphicReqInSystemPerFix->clearGraphs();
    ui->GraphicReqInSystemPerFix->addGraph();
    ui->GraphicReqInSystemPerFix->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicReqInSystemPerFix->graph(0)->setData(time_manager->Time,time_manager->ReqCountInSystemPerFix);

    ui->GraphicReqInSystemPerFix->xAxis->setLabel("Время");
    ui->GraphicReqInSystemPerFix->yAxis->setLabel("Кол-во заявок");

    ui->GraphicReqInSystemPerFix->xAxis->setRange(0, time_manager->CurrentTime);
    ui->GraphicReqInSystemPerFix->yAxis->setRange(0, *max);
    ui->GraphicReqInSystemPerFix->replot();
}

void MainWindow::DrawGraphicPPerFix(TimeManager *time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->PPerFix.begin(), time_manager->PPerFix.end());
    ui->GraphicPPerFix->setInteraction(QCP::iRangeDrag);
    ui->GraphicPPerFix->setInteraction(QCP::iRangeZoom);

    ui->GraphicPPerFix->clearGraphs();
    ui->GraphicPPerFix->addGraph();
    ui->GraphicPPerFix->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicPPerFix->graph(0)->setData(time_manager->Time,time_manager->PPerFix);

    ui->GraphicPPerFix->xAxis->setLabel("Время");
    ui->GraphicPPerFix->yAxis->setLabel("Коэффициент использования системы");

    ui->GraphicPPerFix->xAxis->setRange(0, time_manager->CurrentTime);
    ui->GraphicPPerFix->yAxis->setRange(-1, *max+1);
    ui->GraphicPPerFix->replot();
}

void MainWindow::DrawGraphicExpAdmission(TimeManager *time_manager)
{
     QVector<double>::iterator max = std::max_element(time_manager->ExpRaspAdmission.begin(), time_manager->ExpRaspAdmission.end());
     ui->GraphicExpAdmission->setInteraction(QCP::iRangeDrag);
     ui->GraphicExpAdmission->setInteraction(QCP::iRangeZoom);

     ui->GraphicExpAdmission->clearGraphs();
     ui->GraphicExpAdmission->addGraph();
     ui->GraphicExpAdmission->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

     ui->GraphicExpAdmission->graph(0)->setData(*time_manager->ExpTime,time_manager->ExpRaspAdmission);

     ui->GraphicExpAdmission->xAxis->setLabel("Время");
     ui->GraphicExpAdmission->yAxis->setLabel("Значение функции распределения");

     ui->GraphicExpAdmission->xAxis->setRange(0, *time_manager->Time.rbegin());
     ui->GraphicExpAdmission->yAxis->setRange(0, *max);
     ui->GraphicExpAdmission->replot();
}

void MainWindow::DrawGraphicExpService(TimeManager *time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->ExpRaspService.begin(), time_manager->ExpRaspService.end());
    ui->GraphicExpService->setInteraction(QCP::iRangeDrag);
    ui->GraphicExpService->setInteraction(QCP::iRangeZoom);

    ui->GraphicExpService->clearGraphs();
    ui->GraphicExpService->addGraph();
    ui->GraphicExpService->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicExpService->graph(0)->setData(*time_manager->ExpTime,time_manager->ExpRaspService);

    ui->GraphicExpService->xAxis->setLabel("Время");
    ui->GraphicExpService->yAxis->setLabel("Значение функции распределения");

    ui->GraphicExpService->xAxis->setRange(0, *time_manager->Time.rbegin());
    ui->GraphicExpService->yAxis->setRange(0, *max);
    ui->GraphicExpService->replot();
}

void MainWindow::DrawGraphicExpDensityService(TimeManager *time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->ExpRaspDensityService.begin(), time_manager->ExpRaspDensityService.end());
    ui->GraphicExpDensityService->setInteraction(QCP::iRangeDrag);
    ui->GraphicExpDensityService->setInteraction(QCP::iRangeZoom);

    ui->GraphicExpDensityService->clearGraphs();
    ui->GraphicExpDensityService->addGraph();
    ui->GraphicExpDensityService->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicExpDensityService->graph(0)->setData(*time_manager->ExpTime,time_manager->ExpRaspDensityService);

    ui->GraphicExpDensityService->xAxis->setLabel("Время");
    ui->GraphicExpDensityService->yAxis->setLabel("Значение плотности функции распределения");

    ui->GraphicExpDensityService->xAxis->setRange(0, *time_manager->Time.rbegin());
    ui->GraphicExpDensityService->yAxis->setRange(0, *max);
    ui->GraphicExpDensityService->replot();
}

void MainWindow::DrawGraphicExpDensityAdmission(TimeManager *time_manager)
{
    QVector<double>::iterator max = std::max_element(time_manager->ExpRaspDensityAdmission.begin(), time_manager->ExpRaspDensityAdmission.end());
    ui->GraphicExpDensityAdmission->setInteraction(QCP::iRangeDrag);
    ui->GraphicExpDensityAdmission->setInteraction(QCP::iRangeZoom);

    ui->GraphicExpDensityAdmission->clearGraphs();
    ui->GraphicExpDensityAdmission->addGraph();
    ui->GraphicExpDensityAdmission->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    ui->GraphicExpDensityAdmission->graph(0)->setData(*time_manager->ExpTime,time_manager->ExpRaspDensityAdmission);

    ui->GraphicExpDensityAdmission->xAxis->setLabel("Время");
    ui->GraphicExpDensityAdmission->yAxis->setLabel("Значение плотности функции распределения");

    ui->GraphicExpDensityAdmission->xAxis->setRange(0, *time_manager->Time.rbegin());
    ui->GraphicExpDensityAdmission->yAxis->setRange(0, *max);
    ui->GraphicExpDensityAdmission->replot();
}

void MainWindow::on_pushButton_clicked()
{
    int ReqNeed=ui->TBReqNeed->toPlainText().toInt(), WorkerCount=ui->TBWorkerCount->toPlainText().toInt();
    double AverageAdmissionTime=ui->TBAdmissionTime->toPlainText().toDouble(),
        AverageServiceTime=ui->TBAverageServiceTime->toPlainText().toDouble(),
        ReqLimit=ui->TBReqLimit->toPlainText().toDouble();

    double sum=0.0;
    for (int i=0;i< ui->TLowPriority->model()->rowCount();++i) {
       sum+= ui->TLowPriority->model()->index(i,1).data().toDouble();
    }
    if(!ui->TLowPriority->model()->rowCount()==0 && !TimeManager::TimeEquivalently(sum,1.0)){
        return;
    }
    sum=0;
    for (int i=0;i< ui->THighPriority->model()->rowCount();++i) {
       sum+= ui->THighPriority->model()->index(i,1).data().toDouble();
    }
    if(!ui->THighPriority->model()->rowCount()==0 && !TimeManager::TimeEquivalently(sum,1.0)){
        return;
    }

    TimeManager time_manager(ReqNeed, WorkerCount, 1.0/AverageServiceTime, 1.0/AverageAdmissionTime);
    time_manager.Limit=ReqLimit;
    GetPriorityFromTables(&time_manager);
    //time_manager.LowPriorityArr={{1,{0.0,0.2}}, {2,{0.2,1.0}}};
    //time_manager.HighPriorityArr={{2,{0.0,0.1}},{3,{0.1,0.2}},{4,{0.2,0.8}},{5,{0.8,1.0}}};
    time_manager.TypeRequestBound=ui->LCDTypeRequestBound->value();
    //AverageServiceTime =0.04 AverageReqAdmissionTime=0.1
    time_manager.AddNextReqBeforeSomeTime();

        while (!time_manager.TimeHandle.empty())
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
                event_.workerPtr->ReqState = -1;
                event_.workerPtr->ReqPriority = -1;

                Worker::ProcessedReqQueue.push_back(event_.req);
                time_manager.CheckReqContainer();
            }
        }


        time_manager.SetGraphicsDataExpAdmission();
        time_manager.SetGraphicsDataExpService();
        time_manager.SetGraphicsDataExpDensityAdmission();
        time_manager.SetGraphicsDataExpDensityService();

        ui->TBReqFailedCoint->setText(QString::number(time_manager.ReqFailed.size()));

        ui->TBP ->setText(QString::number(time_manager.GetSystemUtilizationP()));
        ui->TBTq->setText(QString::number(time_manager.GetAverageWaitingTimeForAnApplicationInQueueTq()));
        ui->TBTs->setText(QString::number(time_manager.GetAverageTimeSpentByTheApplicationInTheSystemTs()));
        ui->TBNq->setText(QString::number(time_manager.GetTimeAverageNumberOfRequestsInTheQueueNq()));
        ui->TBNs->setText(QString::number(time_manager.GetTimeAverageNumberOfRequirementsInTheSystemNs()));
        ui->TBCa->setText(QString::number(time_manager.GetAbsoluteSystemCapacityCa()));
        ui->TBCr->setText(QString::number(time_manager.GetAbsoluteSystemCapacityCr()));

        DrawRequestBars(&time_manager);

        DrawGraphicReqInDeqPerFix(&time_manager);
        DrawGraphicReqInSystemPerFix(&time_manager);
        DrawGraphicPPerFix(&time_manager);
        DrawGraphicNq(&time_manager);
        DrawGraphicNs(&time_manager);

        DrawGraphicExpAdmission(&time_manager);
        DrawGraphicExpService(&time_manager);

        DrawGraphicExpDensityAdmission(&time_manager);
        DrawGraphicExpDensityService(&time_manager);

        ShowWorkerInfoTable(&time_manager);
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

void MainWindow::on_pushButtonLowPriority_clicked()
{
    ui->TLowPriority->model()->insertRow(ui->TLowPriority->model()->rowCount(QModelIndex()));
}

void MainWindow::on_pushButtonHighPriority_clicked()
{
    ui->THighPriority->model()->insertRow(ui->THighPriority->model()->rowCount(QModelIndex()));
}

void MainWindow::on_ScrollBarTypeRequestBound_valueChanged(int value)
{    
    ui->LCDTypeRequestBound->display(double(value)/1000);
}

void MainWindow::on_ScrollBarTypeRequestBound_rangeChanged(int min, int max)
{

}

void MainWindow::on_TLowPriority_doubleClicked(const QModelIndex &index)
{
    ui->TLowPriority->model()->removeRow(index.row());
}

void MainWindow::on_THighPriority_doubleClicked(const QModelIndex &index)
{
    ui->THighPriority->model()->removeRow(index.row());
}
