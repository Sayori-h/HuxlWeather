#ifndef MAINWINDOW_H
#define MAINWINDOW_H
// 高温平均值起始坐标
#define TEMPERATURE_STARTING_COORDINATE 30
#define SPAN_INDEX 5// 温度曲线间隔指数
#define ORIGIN_SIZE 3// 温度曲线坐标点大小

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
#include "weatherdata.h"
#include <QMessageBox>
#include <QPainter>
#include <QTimer>

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
    bool eventFilter(QObject *watched,QEvent *event) override;
private slots:
    void slotExitApp();
    void replayFinished(QNetworkReply*reply);
    void on_refreshBt_clicked();

    void on_searchBt_clicked();

private:
    void getWeatherInfo(QNetworkAccessManager *manager);
    void parseJson(QByteArray& bytes);
    void setLabelContent();
    void paintSunRiseSet();
    void paintCurve();
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

    Today today;
    Forecast forecast[6];

    QString url;        // 接口链接
    QString city;       // 访问的城市
    QString cityTmp;    // 临时存放城市变量，防止输入错误城市的时候，原来的城市名称还在。
    WeatherTool tool;   // 天气工具对象
    QNetworkAccessManager *manager;

    static const QPoint sun[2];//直线的两点
    static const QRect sunRiseSet[3];//"日出/日落时间"文本框位置
    static const QRect arcRect;//绘制日出日落圆弧的矩形区域

    QTimer *sunTimer;//通过定时器定时重绘日出日落扇形图
};
#endif // MAINWINDOW_H
