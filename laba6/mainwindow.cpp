#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QLayout>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleNetworkReply);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(ui->lblDashboardTitle, 0, 0, 1, 3, Qt::AlignCenter);
    layout->addWidget(ui->lblCurrentTemperature, 1, 0, 1, 3, Qt::AlignCenter);
    layout->addWidget(ui->chartView, 2, 0, 1, 3);
    layout->addWidget(ui->btnCurrentTemperature, 3, 0);
    layout->addWidget(ui->btnHourlyAverage, 3, 1);
    layout->addWidget(ui->btnDailyAverage, 3, 2);
    ui->centralwidget->setLayout(layout);

    ui->lblDashboardTitle->setAlignment(Qt::AlignCenter);
    ui->lblDashboardTitle->setText("Temperature Dashboard");

    ui->lblCurrentTemperature->setAlignment(Qt::AlignCenter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnCurrentTemperature_clicked()
{
    QNetworkRequest request(QUrl("http://localhost:8080/api/current_temperature"));
    networkManager->get(request);
}

void MainWindow::on_btnHourlyAverage_clicked()
{
    QNetworkRequest request(QUrl("http://localhost:8080/api/average_temperature/hour"));
    networkManager->get(request);
}

void MainWindow::on_btnDailyAverage_clicked()
{
    QNetworkRequest request(QUrl("http://localhost:8080/api/average_temperature/day"));
    networkManager->get(request);
}

void MainWindow::handleNetworkReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        ui->lblCurrentTemperature->setText("Error: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QUrl url = reply->url();

    if (url.path().contains("/current_temperature")) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        QString temperature = QString::number(jsonObj["temperature"].toDouble());
        displayTemperature(temperature);
    } else if (url.path().contains("/average_temperature")) {
        QVector<QPointF> dataPoints = parseJsonArray(responseData);
        QString title = url.path().contains("hour") ? "Hourly Average" : "Daily Average";
        displayChart(dataPoints, title);
    }

    reply->deleteLater();
}

void MainWindow::displayTemperature(const QString &temperature)
{
    ui->lblCurrentTemperature->setText("Current Temperature: " + temperature + " °C");
}

void MainWindow::displayChart(const QVector<QPointF> &dataPoints, const QString &title)
{
    QLayout *layout = ui->chartView->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }

    QLineSeries *series = new QLineSeries();
    for (const QPointF &point : dataPoints) {
        series->append(point);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd HH:mm");
    axisX->setTitleText("Time");
    axisX->setMin(QDateTime::fromMSecsSinceEpoch(dataPoints.first().x()));
    axisX->setMax(QDateTime::fromMSecsSinceEpoch(dataPoints.last().x()));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Temperature (°C)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    series->setColor(Qt::blue);
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setPlotAreaBackgroundBrush(QBrush(Qt::white));
    chart->setPlotAreaBackgroundVisible(true);
    chart->legend()->setVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *newLayout = new QVBoxLayout();
    newLayout->addWidget(chartView);
    ui->chartView->setLayout(newLayout);
}


QVector<QPointF> MainWindow::parseJsonArray(const QByteArray &jsonData)
{
    QVector<QPointF> dataPoints;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isArray()) {
        qDebug() << "Invalid JSON data:" << jsonData;
        return QVector<QPointF>();
    }

    QJsonArray jsonArray = jsonDoc.array();

    for (const QJsonValue &value : jsonArray) {
        QJsonObject obj = value.toObject();
        QDateTime timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
        if (!timestamp.isValid()) {
            qDebug() << "Invalid timestamp format:" << obj["timestamp"].toString();
            continue;
        }
        double temperature = obj["temperature"].toDouble();
        dataPoints.append(QPointF(timestamp.toMSecsSinceEpoch(), temperature));
    }

    return dataPoints;
}
