#include "edidreader.h"
#include <QSettings>

/*
λԪ�M����
    00-19: ���^�YӍ
    00�C07: ���^�YӍ "00h FFh FFh FFh FFh FFh FFh 00h"
    08�C09: �u����ID�� �@���R�e�a�ǽ���΢ܛ����䡣 
    "00001=A���� ��00010=B���� ... ��11010=Z���� ��7λԪ (λַ 08h) ��0�� ��һ����Ԫ(��ĸ)
    ��λ�λԪ 6 �� 2 (λַ 08h)�� �ڶ�����Ԫ(��ĸ)��λ�λԪ1��0 (λַ 08h)��λԪ7 �� 5
    (λַ 09h)����������Ԫ(��ĸ)��λ�λԪ4 �� 0 (λַ 09h)��
    10�C11: ���aID�a (���淽ʽ��LSB�_ʼ)�� ���u���̷��䡣
    12�C15: 32λԪ��̖�� �Ǳ����ʽ�� ͨ�������� LSB ���ȡ�����ȥ�S�ֺ�֮ǰ����������ԣ�����һ��ASCII
    ��̖��Ԕ���r�}���ֱ��ṩ���@����λ��ԓ�O������һ��λԪ�M�����㡣
    16: �u���L�� �@�����u���̸�׃�� ��һ��ȥӋ��һ�µ� 1-7 ������һ�L��һ�� 8-15 �����ڶ��L�K���Դ�
    ��ơ� һЩӋ���������ڎ�(������-������)����A����Ч������ 1-54��
    17: �u����ݡ� ����1990���Ǵ_������ݡ�
    18: EDID �汾̖�a�� "01h"
    19: EDID ��ӆ��̖�a�� "03h"
    20-24: �����@ʾ����
    20: Ӱ��ݔ�붨�x
    λԪ 7�� 0=�, 1=��λ
    ���� λԪ 7 �ǔ�λ��
    λԪ 0�� 1=����DFP 1.x
    ���� λԪ 7 ��ȣ�
    λԪ 6-5�� Ӱ��ȼ�
    00=0.7, 0.3�� 01=0.714, 0.286�� 10=1, 0.4 11=0.7, 0
    bit 4: �׺��O��
    bit 3: ���xͬ��
    bit 2: �ϳ�ͬ��
    bit 1: �Gɫͬ��
    bit 0: ��X��ֱͬ��
    21: ���ˮƽ�D�γߴ� (��λ�鹫��)��
    22: ���ֱ�D�γߴ� (��λ�鹫��)��
    23: �@ʾ٤���� ����100�ټ�1����������ֵ��
    24: �Դ�����֧Ԯ�����磺
    bit 7�� ����
    bit 6�� ��ͣ
    bit 5�� ���S�P�]/���Դ
    bit 4-3�� �@ʾ�͑B
    00=�ڰ�, 01=RGB ɫ��, 10=�� RGB ��ɫ��, 11=δ���x
    bit 2�� �˜�ɫ�ʿ��g
    bit 1�� ƫ�Õr�}ģʽ
    bit 0�� �A�O GTF ֧Ԯ
    25-34: ɫ���{��
    25: ����Чλ�P춼tɫ X1X0 (λԪ 7-6)�� �tɫ Y1Y0 (λԪ 5-4)�� �Gɫ X1X0 (λԪ 3-2)��
    �Gɫ Y1Y0 (λԪ 1-0)��
    26: ����Чλ�P��{ɫ X1X0 (λԪ 7-6)�� �{ɫ Y1Y0 (λԪ 5-4)�� ��ɫ X1X0 (λԪ 3-2)��
    ��ɫ Y1Y0 (λԪ 1-0)��
    27�C34: ����Чλ�P춼tɫ X9-2�� �tɫ Y9-2�� �Gɫ X9-2�� �Gɫ Y9-2�� �{ɫ X9-2�� 
    �{ɫ Y9-2�� ��ɫ X9-2�� ��ɫ Y9-2��
    ���_ֵ�ǽ��0.000��0.999�������aֵ�ǽ��000h��3FFh��
    35: �����r�} I
    λԪ 7-0�� 720��400@70 Hz�� 720��400@88 Hz�� 640��480@60 Hz�� 640��480@67 Hz��
    640��480@72 Hz�� 640��480@75 Hz�� 800��600@56 Hz�� 800��600@60 Hz
    36: �����r�} II
    λԪ 7-0�� 800��600@72 Hz�� 800��600@75 Hz�� 832��624@75 Hz�� 1024��768@87 Hz (���e��)��
    1024��768@60 Hz�� 1024��768@70 Hz�� 1024��768@75 Hz�� 1280��1024@75 Hz
    37: �u���̱����ĕr�}
    00h �ǟo
    λԪ 7�� 1152x870 @ 75 Hz (������ II�� �O��)
    38�C53: �˜ʕr�}�R�e��
    ��һ��λԪ�M
    ˮƽ�Y����  ����31���ٳ���8�� �õ����_ֵ��
    �ڶ���λԪ�M
    λԪ 7-6�� ���^������ ���_�Ĵ�ֱ�Y����هˮƽ�Y����
    00=16��10�� 01=4��3�� 10=5��4�� 11=16��9 (00=1:1 ��v1.3֮ǰ)
    λԪ 5-0�� ��ֱ�l�ʡ� ���� 60 ȥ�õ����_��ֵ��
    54�C71: Ԕ���r�}���� 1
    54�C55: ���ؕr�} (��λ�� 10 kHz) �� 0  (55 MSB  54 LSB)
    �������ؕr�}�K�ǟoЧ��
    56�� ˮƽ���S (��λ������)
    57�� ˮƽ�հ� (��λ������)
    58�� ˮƽ���S�� (4 ��λԪ)
    ˮƽ�հ׸� (4 ��λԪ)
    59�� ��ֱ���S (��λ�龀)
    60�� ��ֱ�հ� (��λ�龀)
    61�� ��ֱ���S�ڸ���Чλ (4 ��λԪ)
    ��ֱ�հ��ڸ���Чλ (4 ��λԪ)
    62�� ˮƽͬ��ƫ���� (��λ������)
    63�� ˮƽͬ���}�_���� (��λ������)
    64�� ��ֱͬ��ƫ���� (��λ�龀) (4 ��λԪ)
    ��ֱͬ���}�_���� (��λ�龀) (4 ��λԪ)
    65�� ����Чλ�P�ˮƽͬ��ƫ���� (λԪ 7-6)
    ����Чλ�P�ˮƽͬ���}�_���� (λԪ 5-4)
    ����Чλ�P춴�ֱͬ��ƫ���� (λԪ 3-2)
    ����Чλ�P춴�ֱͬ���}�_���� (λԪ 1-0)
    66�� ˮƽ�D��ߴ� (��λ�鹫�)
    67�� ��ֱ�D��ߴ� (��λ�鹫�)
    68�� ����Чλ�P�ˮƽ�D��ߴ� (4 ��λԪ)
    ����Чλ�P춴�ֱ�D��ߴ� (4 ��λԪ)
    69�� ˮƽ߅�羀 (��λ��������ֻ��ʾһ߅)
    70�� ��ֱ߅�羀 (��λ�龀��ֻ��ʾһ߅)
    71�� ���e�c�� (λԪ 7)
    �����c�� (λԪ 6-5) ("00" ��ʾ��)
    ���xͬ���c�� (λԪ 4-3)
    ��ֱͬ�����c�� (λԪ 2)
    ˮƽͬ�����c�� (λԪ 1)
    ����ģʽ (λԪ 0) (����6-5 �� 00 �t�]ʹ��)
    �������ؕr�}�ǟoЧ��
    56�� 0
    57�� �^�K�͑B
    FFh=�Oҕ����̖�� FEh=ASCII �ִ��� FDh=�Oҕ��׃�����ƣ� FCh=�Oҕ�����Q��
    FBh=ɫ���c�Y�ϣ� FAh�� �˜ʕr�}�Y�ϣ� F9h=�F��δ���x�� 
    0Fh=���u���̶��x
    58�� 0
    59�C71: �^�K������������
    ����^�K�͑B�� FFh�� FEh�� �� FCh�� �����^�����ִ���
    ����^�K�͑B�� FDh��
    59�C63��
    ��С��ֱ�l�ʣ� ���ֱ�l�ʣ� 
    ��Сˮƽ�l�� (��λ�� kHz)�� ���ˮƽ�l�� (��λ�� kHz)�� ���ؕr�} 
    (��λ�� MHz (���_ֵ�����10))
    64�C65: �ڶ� GTF �|�l��
    ���美�aֵ�� 000A�� λԪ�M 59-63 ��ʹ�á� ���美�aֵ�� 0200�� 
    λԪ�M 67�C71 ��ʹ�á�
    66�� �_ʼˮƽ�l�� (��λ�� kHz)��  ����2�õ����Hֵ��
    67�� C�� ���� 2 �õ����Hֵ��
    68-69�� M (��LSB���ȃ���)��
    70�� K
    71�� J�� ���� 2 �õ����Hֵ��
    ����^�K�͑B�� FBh��
    59�� W ���� 0�� �����O���� 0�� λԪ�M 60-63 �Ǜ]ʹ�á� �����O���� 1�� 61�C63 �� 
    ���䵽���c���� #1
    64�� W ���� 1�� �����O���� 0�� λԪ�M 65-68 �Ǜ]ʹ�á� �����O���� 2�� 65�C68 �� 
    ���䵽���c���� #2
    ���c�����Y����
    ��һ��λԪ
    λԪ 3-2�� ����Чλ�P춰� X (λԪ 3-2)�� �� Y (λԪ 1-0)
    �ڶ�������λԪ�M�� ����Чλ�P춰� X�� �� Y��
    ����λԪ�M�� ٤���� ����100�� �ټ���1�õ����Hֵ��
    ��a�� X �Ͱ� Y�� ��λԪ�M 25-34��
    ����^�K�͑B�� FAh��
    59�C70�� �˜ʕr�}�R�e��  2 λԪ�M���ÿһ���o䛡�
    �P춽Y�������� ��λԪ�M 38-53��
    72�C89: Ԕ���r�}���� 2 ��Oҕ��������
    90�C107: Ԕ���r�}���� 3 ��Oҕ��������
    108�C125: Ԕ���r�}���� 4 ��Oҕ��������
    126: �~�����ˡ� �~��Ĕ�ֵ�����@���^�K��
    ��EDID 1.3֮ǰ�� �@�Ǳ����Եģ� �K�ґ�ԓ���O�� 0��
    127: У�� - �@��λԪ�M��ԓ����ʽ��ʹ������ 128 λԪ�M�ļӿ���� 00h.
*/
struct STUEDID
{
    //00-19: ���^�YӍ
    unsigned char header[8];        // 00�C07: ���^�YӍ "00h FFh FFh FFh FFh FFh FFh 00h"
    unsigned char manufactureId[2]; // 8-9
    unsigned char productionId[2];  // 10-11
    unsigned int  serialNo;         //12-15
    unsigned char WeekofManufacture; //16
    unsigned char YearofManufacture; //17
    unsigned char EDIDVersion; //18
    unsigned char EDIDRevision; //19

    //20-24: �����@ʾ����
    unsigned char Video_i_p_definition;//20
    unsigned char MaxHorzImageSize_CM;//21: ���ˮƽ�D�γߴ� (��λ�鹫��)��
    unsigned char MaxVertImageSize_CM;//22: ���ֱ�D�γߴ� (��λ�鹫��)��
    unsigned char DisplayGamma;//23: �@ʾ٤���� ����100�ټ�1����������ֵ��
    unsigned char PowerManagement_ExtensionBlocs;// 24: �Դ�����֧Ԯ�����磺

    //25-34: ɫ���{��
    //35: �����r�} I
    //36: �����r�} II
    //37: �u���̱����ĕr�}
    //38�C53: �˜ʕr�}�R�e��
    unsigned char ignore1[28];
    //54�C71: Ԕ���r�}���� 1
    unsigned char DetailedTimingDescriptions[17];
    //72�C89: Ԕ���r�}���� 2 ��Oҕ��������
    //90�C107: Ԕ���r�}���� 3 ��Oҕ��������
    //108�C125: Ԕ���r�}���� 4 ��Oҕ��������
    //126: �~�����ˡ� �~��Ĕ�ֵ�����@���^�K��
    //127: У�� - �@��λԪ�M��ԓ����ʽ��ʹ������ 128 λԪ�M�ļӿ���� 00h.
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
            //�ǻ��Ļ��������
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
