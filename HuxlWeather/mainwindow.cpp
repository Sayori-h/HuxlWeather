#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置窗口属性为无边框，没有标题栏，菜单栏需在UI中手动删除
    setWindowFlag(Qt::FramelessWindowHint);
    // 设置窗口固定大小
    setFixedSize(width(),height());

    m_pExitMenu=new QMenu(this);
    m_pExitAct=new QAction(this);
    m_pExitAct->setText(u8"退出系统");
    m_pExitAct->setIcon(QIcon(":/weatherIco/close.ico"));
    m_pExitMenu->addAction(m_pExitAct);
    connect(m_pExitAct,&QAction::triggered,this,&MainWindow::slotExitApp);

    forecast_week_list << ui->week0Lb << ui->week1Lb << ui->week2Lb << ui->week3Lb << ui->week4Lb << ui->week5Lb;
    forecast_date_list << ui->date0Lb << ui->date1Lb << ui->date2Lb << ui->date3Lb << ui->date4Lb << ui->date5Lb;
    forecast_aqi_list << ui->quality0Lb << ui->quality1Lb << ui->quality2Lb << ui->quality3Lb << ui->quality4Lb << ui->quality5Lb;
    forecast_type_list << ui->type0Lb << ui->type1Lb << ui->type2Lb << ui->type3Lb << ui->type4Lb << ui->type5Lb;
    forecast_typeIco_list << ui->typeIco0Lb << ui->typeIco1Lb << ui->typeIco2Lb << ui->typeIco3Lb << ui->typeIco4Lb << ui->typeIco5Lb;
    forecast_high_list << ui->high0Lb << ui->high1Lb << ui->high2Lb << ui->high3Lb << ui->high4Lb << ui->high5Lb;
    forecast_low_list << ui->low0Lb << ui->low1Lb << ui->low2Lb << ui->low3Lb << ui->low4Lb << ui->low5Lb;

    //请求天气API信息
    url="http://t.weather.itboy.net/api/weather/city/";
    city=u8"南京";
    cityTmp=city;
    manager=new QNetworkAccessManager(this);
    connect(manager,&QNetworkAccessManager::finished,this,&MainWindow::replayFinished);
    getWeatherInfo(manager);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    m_pExitMenu->exec(QCursor::pos());// 弹出右键菜单
    event->accept();// 将右键菜单事件标记为已接受
}

// 窗口移动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_Pos=event->globalPos()-this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPos()-m_Pos);
}

void MainWindow::slotExitApp()
{
    qApp->exit(0);//退出应用程序
}

void MainWindow::replayFinished(QNetworkReply *reply)
{
    // 获取响应的信息，状态码为200表示正常
    QVariant statusCode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    // 检查是否可以转换为整数
    bool ok;
    int statusCodeValue = statusCode.toInt(&ok);

    if(reply->error()!=QNetworkReply::NoError||(!ok)||(statusCodeValue!=200)){
        QMessageBox::critical(this,u8"错误",u8"天气：请求数据错误，检查网络连接！");
        return;
    }
    QByteArray bytes=reply->readAll();
    //qDebug()<<bytes;
    parseJson(bytes);
}

void MainWindow::getWeatherInfo(QNetworkAccessManager *manager)
{
    QString cityCode=tool[city];
    if(cityCode=="000000000"){
        QMessageBox::critical(this,u8"错误",u8"天气：指定城市不存在！");
        return;
    }
    QUrl jsonUrl(url+cityCode);
    manager->get(QNetworkRequest(jsonUrl));
}

/* 解析Json数据 */
void MainWindow::parseJson(QByteArray& bytes)
{
    //读取Json数据
    QJsonParseError err;
    QJsonDocument jsonDoc=QJsonDocument::fromJson(bytes,&err);
    if(err.error!=QJsonParseError::NoError)return;

    QJsonObject jsObj=jsonDoc.object();
    qDebug()<<jsObj;
    qDebug()<<"==========================================\r\n";
    qDebug()<<jsObj.value("message").toString();
    QString message=jsObj.value("message").toString();
    if(!message.contains("success")){
        QMessageBox::information(this,u8"来自Json的信息",u8"天气：城市错误！",QMessageBox::Ok);
        city=cityTmp;
        return;
    }
    today=jsObj;

    //解析data中的yesterday
    QJsonObject dataObj=jsObj.value("data").toObject();
    forecast[0]=dataObj.value("yesterday").toObject();

    //解析data中的forecast
    QJsonArray forecastArr=dataObj.value("foecast").toArray();
    int j=0;
    for(int i=1;i<6;i++){
        forecast[i]=forecastArr.at(j).toObject();
        j++;
    }
}

