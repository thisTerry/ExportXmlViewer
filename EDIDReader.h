#ifndef EDIDREADER_H
#define EDIDREADER_H

#include <QObject>
#include <QStringList>

class EDIDReader : public QObject
{
    Q_OBJECT

public:
    EDIDReader(QObject *parent=NULL);
    ~EDIDReader();

    bool HasMultiScreen();
    QString GetManufactureName();

    qreal GetScreenWidthMM();
    qreal GetScreenHeightMM();

    qint32 GetScreenWidth();
    qint32 GetScreenHeight();

private:
    void Load();
    QStringList GetChidGroups(const QString & path);
    QStringList GetChildKeys(const QString & path, QVariantList& valueList=QVariantList());
    bool LoadActiveDisplayInfo();
    void ParseActiveDisplayInfo(unsigned char * edid);

private:
    QVariantList m_activeDisplayEDIDList;//���л��Ļ��EDID��Ϣ

    //���ж����Ļʱ��ֻȡһ���������
    QString m_manufactureName;
    int m_screenWidthCM;
    int m_screenHeightCM;
    int m_screenWidthPixels;
    int m_screenHeightCMPixels;
};

#endif // EDIDREADER_H
