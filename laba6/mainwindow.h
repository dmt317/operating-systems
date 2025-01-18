#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnCurrentTemperature_clicked();
    void on_btnHourlyAverage_clicked();
    void on_btnDailyAverage_clicked();
    void handleNetworkReply(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    void displayTemperature(const QString &temperature);
    void displayChart(const QVector<QPointF> &dataPoints, const QString &title);
    QVector<QPointF> parseJsonArray(const QByteArray &jsonData);
};

#endif // MAINWINDOW_H
