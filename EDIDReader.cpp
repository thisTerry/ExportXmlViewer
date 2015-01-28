#include "edidreader.h"
#include <QSettings>

/*
位元組序列
    00-19: 標頭資訊
    00–07: 標頭資訊 "00h FFh FFh FFh FFh FFh FFh 00h"
    08–09: 製造商ID。 這個識別碼是經由微軟來分配。 
    "00001=A”； “00010=B”； ... “11010=Z”。 第7位元 (位址 08h) 是0， 第一個字元(字母)
    是位於位元 6 → 2 (位址 08h)， 第二個字元(字母)是位於位元1和0 (位址 08h)和位元7 → 5
    (位址 09h)，第三個字元(字母)是位於位元4 → 0 (位址 09h)。
    10–11: 生產ID碼 (儲存方式是LSB開始)。 由製造商分配。
    12–15: 32位元序號。 非必需格式。 通常儲存由 LSB 優先。為了去維持和之前需求的相容性，假如一個ASCII
    序號在詳細時脈部分被提供，這個欄位應該設定至少一個位元組不是零。
    16: 製造週。 這個由製造商改變。 法一是去計算一月的 1-7 當做第一週，一月 8-15 當做第二週並且以此
    類推。 一些計算是以星期幾(星期日-星期六)為基礎。有效範圍是 1-54。
    17: 製造年份。 加上1990才是確實的年份。
    18: EDID 版本號碼。 "01h"
    19: EDID 修訂版號碼。 "03h"
    20-24: 基本顯示參數
    20: 影像輸入定義
    位元 7： 0=類比, 1=數位
    假如 位元 7 是數位：
    位元 0： 1=相容DFP 1.x
    假如 位元 7 是類比：
    位元 6-5： 影像等級
    00=0.7, 0.3， 01=0.714, 0.286， 10=1, 0.4 11=0.7, 0
    bit 4: 白黑設定
    bit 3: 分離同步
    bit 2: 合成同步
    bit 1: 綠色同步
    bit 0: 鋸齒垂直同步
    21: 最大水平圖形尺寸 (單位為公分)。
    22: 最大垂直圖形尺寸 (單位為公分)。
    23: 顯示伽瑪。 除以100再加1才是真正的值。
    24: 電源管理和支援的特徵：
    bit 7： 待命
    bit 6： 暫停
    bit 5： 活躍關閉/低電源
    bit 4-3： 顯示型態
    00=黑白, 01=RGB 色彩, 10=非 RGB 多色彩, 11=未定義
    bit 2： 標準色彩空間
    bit 1： 偏好時脈模式
    bit 0： 預設 GTF 支援
    25-34: 色度調節
    25: 低有效位關於紅色 X1X0 (位元 7-6)， 紅色 Y1Y0 (位元 5-4)， 綠色 X1X0 (位元 3-2)，
    綠色 Y1Y0 (位元 1-0)。
    26: 低有效位關於藍色 X1X0 (位元 7-6)， 藍色 Y1Y0 (位元 5-4)， 白色 X1X0 (位元 3-2)，
    白色 Y1Y0 (位元 1-0)。
    27–34: 高有效位關於紅色 X9-2， 紅色 Y9-2， 綠色 X9-2， 綠色 Y9-2， 藍色 X9-2， 
    藍色 Y9-2， 白色 X9-2， 白色 Y9-2。
    正確值是介於0.000和0.999，但編碼值是介於000h和3FFh。
    35: 建立時脈 I
    位元 7-0： 720×400@70 Hz， 720×400@88 Hz， 640×480@60 Hz， 640×480@67 Hz，
    640×480@72 Hz， 640×480@75 Hz， 800×600@56 Hz， 800×600@60 Hz
    36: 建立時脈 II
    位元 7-0： 800×600@72 Hz， 800×600@75 Hz， 832×624@75 Hz， 1024×768@87 Hz (交錯的)，
    1024×768@60 Hz， 1024×768@70 Hz， 1024×768@75 Hz， 1280×1024@75 Hz
    37: 製造商保留的時脈
    00h 是無
    位元 7： 1152x870 @ 75 Hz (麥金塔 II， 蘋果)
    38–53: 標準時脈識別。
    第一個位元組
    水平結果。  加上31，再乘上8， 得到正確值。
    第二個位元組
    位元 7-6： 外觀比例。 正確的垂直結果依賴水平結果。
    00=16：10， 01=4：3， 10=5：4， 11=16：9 (00=1:1 在v1.3之前)
    位元 5-0： 垂直頻率。 加上 60 去得到正確的值。
    54–71: 詳細時脈描述 1
    54–55: 像素時脈 (單位為 10 kHz) 或 0  (55 MSB  54 LSB)
    假如像素時脈並非無效：
    56： 水平活躍 (單位為像素)
    57： 水平空白 (單位為像素)
    58： 水平活躍高 (4 高位元)
    水平空白高 (4 低位元)
    59： 垂直活躍 (單位為線)
    60： 垂直空白 (單位為線)
    61： 垂直活躍在高有效位 (4 高位元)
    垂直空白在高有效位 (4 低位元)
    62： 水平同步偏移量 (單位為像素)
    63： 水平同步脈沖寬度 (單位為像素)
    64： 垂直同步偏移量 (單位為線) (4 高位元)
    垂直同步脈沖寬度 (單位為線) (4 低位元)
    65： 高有效位關於水平同步偏移量 (位元 7-6)
    高有效位關於水平同步脈沖寬度 (位元 5-4)
    高有效位關於垂直同步偏移量 (位元 3-2)
    高有效位關於垂直同步脈沖寬度 (位元 1-0)
    66： 水平圖像尺寸 (單位為公釐)
    67： 垂直圖像尺寸 (單位為公釐)
    68： 高有效位關於水平圖像尺寸 (4 高位元)
    高有效位關於垂直圖像尺寸 (4 低位元)
    69： 水平邊界線 (單位為像素且只表示一邊)
    70： 垂直邊界線 (單位為線且只表示一邊)
    71： 交錯與否 (位元 7)
    立体與否 (位元 6-5) ("00" 表示否)
    分離同步與否 (位元 4-3)
    垂直同步正與否 (位元 2)
    水平同步正與否 (位元 1)
    立体模式 (位元 0) (若是6-5 是 00 則沒使用)
    假如像素時脈是無效：
    56： 0
    57： 區塊型態
    FFh=監視器序號， FEh=ASCII 字串， FDh=監視器變動限制， FCh=監視器名稱，
    FBh=色彩點資料， FAh， 標準時脈資料， F9h=現在未定義， 
    0Fh=由製造商定義
    58： 0
    59–71: 區塊內容描述符。
    假如區塊型態是 FFh， FEh， 或 FCh， 整個區域是字串。
    假如區塊型態是 FDh：
    59–63：
    最小垂直頻率， 最大垂直頻率， 
    最小水平頻率 (單位為 kHz)， 最大水平頻率 (單位為 kHz)， 像素時脈 
    (單位為 MHz (正確值需乘上10))
    64–65: 第二 GTF 觸發器
    假如編碼值是 000A， 位元組 59-63 是使用。 假如編碼值是 0200， 
    位元組 67–71 是使用。
    66： 開始水平頻率 (單位為 kHz)。  乘上2得到實際值。
    67： C。 除以 2 得到實際值。
    68-69： M (以LSB優先儲存)。
    70： K
    71： J。 除以 2 得到實際值。
    假如區塊型態是 FBh：
    59： W 索引 0。 假如設定成 0， 位元組 60-63 是沒使用。 假如設定成 1， 61–63 是 
    分配到白點索引 #1
    64： W 索引 1。 假如設定成 0， 位元組 65-68 是沒使用。 假如設定成 2， 65–68 是 
    分配到白點索引 #2
    白點索引結構：
    第一個位元
    位元 3-2： 低有效位關於白 X (位元 3-2)， 白 Y (位元 1-0)
    第二到第三位元組： 高有效位關於白 X， 白 Y。
    第四位元組： 伽瑪。 除以100， 再加上1得到實際值。
    解碼白 X 和白 Y， 看位元組 25-34。
    假如區塊型態是 FAh：
    59–70： 標準時脈識別。  2 位元組對於每一個紀錄。
    關於結構細節， 看位元組 38-53。
    72–89: 詳細時脈描述 2 或監視器描述符
    90–107: 詳細時脈描述 3 或監視器描述符
    108–125: 詳細時脈描述 4 或監視器描述符
    126: 額外的旗標。 額外的數值採用這個區塊。
    在EDID 1.3之前， 這是被忽略的， 並且應該被設成 0。
    127: 校驗和 - 這個位元組應該被程式化使得所有 128 位元組的加總等於 00h.
*/
struct STUEDID
{
    //00-19: 標頭資訊
    unsigned char header[8];        // 00–07: 標頭資訊 "00h FFh FFh FFh FFh FFh FFh 00h"
    unsigned char manufactureId[2]; // 8-9
    unsigned char productionId[2];  // 10-11
    unsigned int  serialNo;         //12-15
    unsigned char WeekofManufacture; //16
    unsigned char YearofManufacture; //17
    unsigned char EDIDVersion; //18
    unsigned char EDIDRevision; //19

    //20-24: 基本顯示參數
    unsigned char Video_i_p_definition;//20
    unsigned char MaxHorzImageSize_CM;//21: 最大水平圖形尺寸 (單位為公分)。
    unsigned char MaxVertImageSize_CM;//22: 最大垂直圖形尺寸 (單位為公分)。
    unsigned char DisplayGamma;//23: 顯示伽瑪。 除以100再加1才是真正的值。
    unsigned char PowerManagement_ExtensionBlocs;// 24: 電源管理和支援的特徵：

    //25-34: 色度調節
    //35: 建立時脈 I
    //36: 建立時脈 II
    //37: 製造商保留的時脈
    //38–53: 標準時脈識別。
    unsigned char ignore1[28];
    //54–71: 詳細時脈描述 1
    unsigned char DetailedTimingDescriptions[17];
    //72–89: 詳細時脈描述 2 或監視器描述符
    //90–107: 詳細時脈描述 3 或監視器描述符
    //108–125: 詳細時脈描述 4 或監視器描述符
    //126: 額外的旗標。 額外的數值採用這個區塊。
    //127: 校驗和 - 這個位元組應該被程式化使得所有 128 位元組的加總等於 00h.
    unsigned char ignore2[55];
};

static const QString DISPLAY_PATH = QString("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\");



EDIDReader::EDIDReader(QObject *parent)
    : QObject(parent)
    , m_manufactureName("")
    , m_screenWidthCM(0)
    , m_screenHeightCM(0)
    , m_screenWidthPixels(0)
    , m_screenHeightCMPixels(0)
{
    Load();
}

EDIDReader::~EDIDReader()
{

}

void EDIDReader::Load()
{
    if (LoadActiveDisplayInfo())
    {
        foreach(QVariant var, m_activeDisplayEDIDList)
        {
            STUEDID edid;
            int minSize = sizeof(edid);
            QString str;
            if (QVariant::String == var.type())
            {
                str = var.toString();
                //minSize = (minSize < str.size()) ? minSize : str.size();            
                memcpy (&edid, str.data(), minSize);

                //////////////////////////////////////////////////////////////////////////
                m_screenWidthCM = edid.MaxHorzImageSize_CM;
                m_screenHeightCM = edid.MaxVertImageSize_CM;

                //////////////////////////////////////////////////////////////////////////
                unsigned char * edidArray = (unsigned char *)&edid;
                ParseActiveDisplayInfo(edidArray);
            }
        }
    }
    else
    {
        Q_ASSERT(false);
    }
}


QStringList EDIDReader::GetChidGroups(const QString & path)
{
    QSettings reg(path, QSettings::NativeFormat);
    return reg.childGroups();  
}

QStringList EDIDReader::GetChildKeys(const QString & path, QVariantList& valueList)
{
    QSettings reg(path, QSettings::NativeFormat);
    QStringList keys = reg.childKeys();
    valueList.clear();

    foreach(QString key, keys)
    {
        QVariant value=reg.value(key);
        valueList.append(value);
    }
    return keys;  
}

bool EDIDReader::LoadActiveDisplayInfo()
{
    //m_strActiveDisplayEDID
    QStringList displayList = GetChidGroups(DISPLAY_PATH);
    foreach(QString display,displayList)  
    {
        QStringList typeList = GetChidGroups(DISPLAY_PATH + display);
        if (typeList.isEmpty())
        {
            continue;
        }

        QVariantList controlValues;
        QStringList controlKeys = GetChildKeys(
            DISPLAY_PATH + display +"\\"+ typeList.at(0) +"\\"+ "Control",
            controlValues);

        bool bActiveScreen = false;
        foreach (QString str, controlKeys)
        {
            if (str.contains("ActiveService"))
            {
                bActiveScreen = true;
            }
        }
        if (!bActiveScreen)
        {
            //非活动屏幕，则跳过
            continue;
        }

        QVariantList deviceParametersValues;
        QStringList deviceParametersKeys = GetChildKeys(
            DISPLAY_PATH + display +"\\"+ typeList.at(0) +"\\"+ "Device Parameters",
            deviceParametersValues);

        qint32 indexOfEDID = deviceParametersKeys.indexOf("EDID");
        if (-1 != indexOfEDID)
        {
            m_activeDisplayEDIDList.push_back(deviceParametersValues.at(indexOfEDID));
        }
        else
        {
            Q_ASSERT(false);
        }
    }

    return (m_activeDisplayEDIDList.size()>0);
}


void EDIDReader::ParseActiveDisplayInfo(unsigned char * edid)
{
    //////////////////////////////////////////////////////////////////////////
    char modearray[128][128];
    int currentmode = 0;

    int i;

    //////////////////////////////////////////////////////////////////////////
    int j;
    char modelname[13];
    //check header
    for (i = 0; i < 8; i++) {
        if (!(((i == 0 || i == 7) && edid[i] == 0x00) || (edid[i] == 0xff))) //0x00 0xff 0xff 0xff 0xff 0xff 0x00
            qDebug("Header incorrect. Probably not an edid\n");
    }

    //Make a monitor section...
    qDebug("Section \"Monitor\"\n");

    //Product Identification
    /* Model Name: Only thing I do out of order of edid, to comply with X standards... */
    for (i = 0x36; i < 0x7E; i += 0x12) { //read through descriptor blocks...
        if (edid[i] == 0x00) { //not a timing descriptor
            if (edid[i+3] == 0xfc) { //Model Name tag
                for (j = 0; j < 13; j++) {
                    if (edid[i+5+j] == 0x0a)
                        modelname[j] = 0x00;
                    else
                        modelname[j] = edid[i+5+j];
                }
            }
        }
    }
    qDebug("\tIdentifier \"%s\"\n\tModelName \"%s\"\n", modelname, modelname);


    /* Vendor Name: 3 characters, standardized by microsoft, somewhere.
	 * bytes 8 and 9: f e d c b a 9 8  7 6 5 4 3 2 1 0
	 * Character 1 is e d c b a
	 * Character 2 is 9 8 7 6 5
	 * Character 3 is 4 3 2 1 0
	 * Those values start at 0 (0x00 is 'A', 0x01 is 'B', 0x19 is 'Z', etc.)
	 */
	qDebug("\tVendorName \"%c%c%c\"\n", (edid[8] >> 2 & 0x1f) + 'A' - 1, (((edid[8] & 0x3) << 3) | ((edid[9] & 0xe0) >> 5)) + 'A' - 1, (edid[9] & 0x1f) + 'A' - 1 );

    m_manufactureName.append(QString(tr("%1%2%3"))
        .arg((QChar)((edid[8] >> 2 & 0x1f) + 'A' - 1))
        .arg((QChar)((((edid[8] & 0x3) << 3) | ((edid[9] & 0xe0) >> 5)) + 'A' - 1))
        .arg((QChar)((edid[9] & 0x1f) + 'A' - 1 )));

    //////////////////////////////////////////////////////////////////////////
    for (i = 0x36; i < 0x7E; i += 0x12) { //read through descriptor blocks...
        if (edid[i] == 0x00) { //not a timing descriptor
            if (edid[i+3] == 0xfd) { //monitor limits tag
                qDebug("\tHorizsync %i-%i\n", (int)edid[i+7] + (((edid[i+4] & 0x0c) & 0x04) ? 255 : 0), (int)edid[i+8] + ((edid[i+4] & 0x08) ? 255 : 0));
                qDebug("\tVertRefresh %i-%i\n", (int)edid[i+5] + (((edid[i+4] & 0x03) & 0x01) ? 255 : 0), (int)edid[i+6] + ((edid[i+4] & 0x02) ? 255 : 0));
                qDebug("\t# Maximum pixel clock is %iMHz\n", (int)edid[i+9] * 10);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    int hactive, vactive, pixclk, hsyncoff, hsyncwidth, hblank, vsyncoff, vsyncwidth, vblank;
    //Parse for Detailed Timing Descriptors...
    for (i = 0x36; i < 0x7E; i += 0x12) { //read through descriptor blocks...
        if ((edid[i] != 0x00) && (edid[i+1] != 0x00)) { //a dtd
            hactive = edid[i+2] + ((edid[i+4] & 0xf0) << 4);
            hblank = edid[i+3] + ((edid[i+4] & 0x0f) << 8);
            vactive = edid[i+5] + ((edid[i+7] & 0xf0) << 4);
            vblank = edid[i+6] + ((edid[i+7] & 0x0f) << 8);

            //printf("\tModeline \t\"%dx%d\" ", hactive, vactive);
            qDebug("\tModeline \t\"%dx%d\" ", hactive, vactive);

            //////////////////////////////////////////////////////////////////////////
            m_screenWidthPixels = hactive;
            m_screenHeightCMPixels = vactive;
            //////////////////////////////////////////////////////////////////////////
            

            pixclk = (edid[i+1] << 8) | (edid[i]);

            sprintf_s(modearray[currentmode], sizeof(modearray[currentmode])-1, "%.2f ", (double)pixclk / 100.0);

            //I'm using Fremlin's nomenclature...
            //sync offset = front porch
            //sync width = sync pulse width

            hsyncoff = edid[i+8] | ((edid[i+11] & 0xC0) << 2);
            hsyncwidth = edid[i+9] | ((edid[i+11] & 0x30) << 4);
            vsyncoff = ((edid[i+10] & 0xf0) >> 4) | ((edid[i+11] & 0x0C) << 2);
            vsyncwidth = (edid[i+10] & 0x0f) | ((edid[i+11] & 0x03) << 4);


            sprintf_s(modearray[currentmode], sizeof(modearray[currentmode])-1, "%s%u %u %u %u ", modearray[currentmode], hactive, hactive+hsyncoff, hactive+hsyncoff+hsyncwidth, hactive+hblank);
            sprintf_s(modearray[currentmode], sizeof(modearray[currentmode])-1, "%s%u %u %u %u ", modearray[currentmode], vactive, vactive+vsyncoff, vactive+vsyncoff+vsyncwidth, vactive+vblank);

            if ( (edid[i+17]&0x80) || ((edid[i+17]&0x18) == 0x18) ) {
                sprintf_s(modearray[currentmode], sizeof(modearray[currentmode])-1, "%s%shsync %svsync %s", modearray[currentmode], ((edid[i+17]&0x10) && edid[i+17]&0x02) ? "+": "-", ((edid[i+17]&0x10) && edid[i+17]&0x04) ? "+": "-", (edid[i+17]&0x80) ? "interlace": "");
                //hehe... there's been at least 2 bugs in the old parse-edid the whole time - somebody caught the htimings one, and I just caught two problems right here - lack of checking for analog sync and getting hsync and vsync backwards... yes, vsync and hsync have been flipped this whole time. Glad I'm rewriting
                //printf("%s\n", modearray[currentmode]);
                qDebug("%s\n", modearray[currentmode]);
                currentmode++;

            }
            //printf("\tEndmode\n");
            qDebug("\tEndmode\n");
        }
    }
}

bool EDIDReader::HasMultiScreen()
{
    return (m_activeDisplayEDIDList.size()>1);
}

QString EDIDReader::GetManufactureName()
{
    return m_manufactureName;
}

qreal EDIDReader::GetScreenWidthMM()
{
    return m_screenWidthCM * 10.0;
}

qreal EDIDReader::GetScreenHeightMM()
{
    return m_screenHeightCM * 10.0;
}

qint32 EDIDReader::GetScreenWidth()
{
    return m_screenWidthPixels;
}

qint32 EDIDReader::GetScreenHeight()
{
    return m_screenHeightCMPixels;
}
