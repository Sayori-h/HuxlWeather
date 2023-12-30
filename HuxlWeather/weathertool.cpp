#include "weathertool.h"

WeatherTool::WeatherTool()
{
    //获取当前程序运行的路径
    QString filename=QCoreApplication::applicationDirPath();
    //不添加到资源文件是因为城市数据可能更新，图标不会更新
    filename+="/citycode-2019-08-23.json";
    QFile file(filename);

    //尝试打开文件
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        // 如果文件打开失败，抛出异常
        throw std::runtime_error("Failed to open the file.");

    QByteArray json=file.readAll();
    file.close();
    //读取Json数据
    QJsonParseError err;
    QJsonDocument jsonDoc=QJsonDocument::fromJson(json,&err);
    //获取城市列表数组，当前这个JSON是一个array，Doc转成array
    QJsonArray citys=jsonDoc.array();
    for(int i=0;i<citys.size();i++){
        QString code=citys.at(i).toObject().value("city_code").toString();
        QString city=citys.at(i).toObject().value("city_name").toString();
        //省份的code是空的，所以城市代码长度大于0的都可以添加的map中
        if(code.size()>0)m_mapCity2Code[city]=code;
    }
}

QString WeatherTool::operator[](const QString &city)
{
    auto it=m_mapCity2Code.find(city);
    if(it==m_mapCity2Code.end())
        it=m_mapCity2Code.find(city+u8"市");
    if(it==m_mapCity2Code.end())
        it=m_mapCity2Code.find(city+u8"县");
    if(it!=m_mapCity2Code.end()) return it->second;
    return "000000000";
}
