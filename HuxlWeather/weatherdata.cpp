#include "weatherdata.h"

Today::Today()
{
    date = "0000-00-00";
    city = "null";
    fl = u8"无数据";
    fx = u8"无数据";
    ganmao = u8"无数据";
    notice = u8"无数据";
    pm25 = u8"无数据";
    quality = u8"无数据";
    shidu = u8"无数据";
    sunrise = "00:00";
    sunset = "00:00";
    wendu = "null";
    type = u8"无数据";
}

/*{"message":"success感谢又拍云(upyun.com)提供CDN赞助",
 * "status":200,"date":"20231229","time":"2023-12-29 15:03:13",
 * "cityInfo":{"city":"蛟河市","citykey":"101060204",
 * "parent":"吉林市","updateTime":"10:31"},
 * "data":{"shidu":"100%","pm25":12.0,"pm10":0.0,"quality":"优","wendu":"-17","ganmao":"各类人群可自由活动",
 * "forecast":[{"date":"29","high":"高温 -7℃","low":"低温 -20℃","ymd":"2023-12-29",
 * "week":"星期五","sunrise":"07:04","sunset":"16:01","aqi":23,"fx":"南风","fl":"1级",
 * "type":"多云","notice":"阴晴之间，谨防紫外线侵扰"},...*/

Today &Today::operator=(const QJsonObject &obj)
{
    QString dateStr=obj.value("date").toString();
    date=QDate::fromString(dateStr,"yyyyMMdd").toString("yyyy-MM-dd");

    city=obj.value("cityInfo").toObject().value("city").toString();

    // 解析data
    QJsonObject dataObj=obj.value("data").toObject();
    shidu=dataObj.value("shidu").toString();
    pm25=QString::number(dataObj.value("pm25").toDouble());
    quality=dataObj.value("quality").toString();
    wendu=dataObj.value("wendu").toString()+u8"°";
    ganmao=dataObj.value("ganmao").toString();


    // 解析forecast中today部分
    QJsonArray forecastArr=dataObj.value("forecast").toArray();
    QJsonObject todayObj=forecastArr.at(0).toObject();
    fx=todayObj.value("fx").toString();
    fl=todayObj.value("fl").toString();
    type=todayObj.value("type").toString();
    sunrise=todayObj.value("sunrise").toString();
    sunset = todayObj.value("sunset").toString();
    notice = todayObj.value("notice").toString();
    return *this;
}


Forecast::Forecast()
{
    aqi = "0";
    date = u8"00日星期0";
    week = u8"星期0";
    high = u8"高温 0.0℃";
    low = u8"低温 0.0℃";
    type = "undefined";
}

// 解析forecast中其他部分
Forecast &Forecast::operator=(const QJsonObject &obj)
{
    date = obj.value("date").toString();
    week = obj.value("week").toString();
    high = obj.value("high").toString();
    low = obj.value("low").toString();
    aqi = QString::number( obj.value("aqi").toDouble() );
    type = obj.value("type").toString();
    return *this;
}


