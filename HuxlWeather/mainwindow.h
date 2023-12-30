#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include "weathertool.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private slots:
    void slotExitApp();
    void replayFinished(QNetworkReply*reply);
private:
    void getWeatherInfo(QNetworkAccessManager *manager);
private:
    Ui::MainWindow *ui;
    QMenu *m_pExitMenu;
    QAction *m_pExitAct;
    QPoint m_Pos;

    QList<QLabel *> forecast_week_list;         // 星期
    QList<QLabel *> forecast_date_list;         // 日期
    QList<QLabel *> forecast_aqi_list;          // 天气指数
    QList<QLabel *> forecast_type_list;         // 天气
    QList<QLabel *> forecast_typeIco_list;      // 天气图标
    QList<QLabel *> forecast_high_list;         // 高温
    QList<QLabel *> forecast_low_list;          // 低温

    QString url;        // 接口链接
    QString city;       // 访问的城市
    QString cityTmp;    // 临时存放城市变量，防止输入错误城市的时候，原来的城市名称还在。
    WeatherTool tool;   // 天气工具对象
    QNetworkAccessManager *manager;
};
#endif // MAINWINDOW_H
