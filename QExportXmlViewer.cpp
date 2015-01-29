#include "QExportXmlViewer.h"
#include <QtWidgets/QMenuBar>
#include <QActionGroup>
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollArea>
#include "QExportXmlParser.h"
#include "qexportxmlstruct.h"
#include "qwaveinfodlg.h"
#include "qdisplaysettingdlg.h"
#include "CFGMgr.h"
#include <QFileInfo>
#include "qwaveswidget.h"
#include "qparameterlistwidget.h"
#include "qpatientinfowidget.h"
#ifdef Q_OS_WIN
#include "edidreader.h"
#endif

using namespace psimpl;

static const QString EXPORT_XML_VIEWER               = QString("Export Xml Viewer");
static const QString MEDIANBEATTEMPLATES_INFORMATION = QString("MedianBeatTemplates information");


/*!
    构造函数
    @para[in]  QWidget * parent  
    @para[in]  Qt::WFlags flags  
    @return  
*/
QExportXmlViewer::QExportXmlViewer(QWidget *parent)
	: QMainWindow(parent)
	, m_parserPtr(NULL)
	, m_infoLabelPtr(NULL)
	, m_wavesWidgetPtr(NULL)
	, m_parameterListWidgetPtr(NULL)
	, m_patientInfoWidgetPtr(NULL)
	, m_pCfgMgr(CFGMgr::Instance())
{
	ui.setupUi(this);

	//更新配置值,需要放在InitDialog的前面
    UpdateDisplayPixelsPerCM();

	OnInitDialog();
}


/*!
    析构函数
    @return  
*/
QExportXmlViewer::~QExportXmlViewer()
{
	if (NULL != m_parserPtr)
	{
		delete m_parserPtr;
		m_parserPtr = NULL;
	}
}


/*!
    初始化界面显示。连接信号等。
    @return  void
*/
void QExportXmlViewer::OnInitDialog()
{
	//窗口标题
	setWindowTitle(EXPORT_XML_VIEWER);
	setWindowIcon(QIcon(":/exportxmlviewer/app.png"));

	//工具栏
	ui.toolBar->addAction(ui.action_Open_Xml_File);
	ui.toolBar->addSeparator();
	ui.toolBar->addAction(ui.action_Show_Wave_Info);
	ui.toolBar->addSeparator();
	ui.toolBar->addAction(ui.action_Draw_Waveforms);
	ui.toolBar->addAction(ui.action_Draw_MedianBeatTemplates);
	ui.toolBar->addSeparator();
	ui.toolBar->addAction(ui.action_List_Observations);
	ui.toolBar->addAction(ui.action_Patient_Information);

	//互斥菜单项
	QActionGroup* actionGroup = new QActionGroup(this);
	actionGroup->addAction(ui.action_Draw_Waveforms);
	actionGroup->addAction(ui.action_Draw_MedianBeatTemplates);
	actionGroup->addAction(ui.action_List_Observations);
	actionGroup->addAction(ui.action_Patient_Information);
	actionGroup->setExclusive(true);

	InitCentralWidget();

	OnWarningInformation("");

	//信号连接
	connect(ui.action_Open_Xml_File, SIGNAL(triggered()), this, SLOT(OnOpenClicked()));
	connect(ui.action_Quit, SIGNAL(triggered()), this, SLOT(OnQuitClicked()));
	connect(ui.action_About, SIGNAL(triggered()), this, SLOT(OnAboutClicked()));
	connect(ui.action_Show_Wave_Info, SIGNAL(triggered()), this, SLOT(OnShowWaveInfoClicked()));
	connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(OnActionGroupTriggered(QAction*)));
	connect(ui.action_Display_Settings, SIGNAL(triggered()), this, SLOT(OnDisplaySettingsClicked()));
	connect(m_wavesWidgetPtr, SIGNAL(WarningInformation(const QString&)), this, SLOT(OnWarningInformation(const QString&)));
	
	//隐藏所有控件
	HideAllWidget();
}

void QExportXmlViewer::InitCentralWidget()
{
	QVBoxLayout* verticalLayout = new QVBoxLayout(ui.centralWidget);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);

	ui.centralWidget->layout()->setSizeConstraint(QLayout::SetFixedSize);

	//信息提示控件
	m_infoLabelPtr = new QLabel(ui.centralWidget);
	m_infoLabelPtr->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
	verticalLayout->addWidget(m_infoLabelPtr);

	//波形控件
	m_wavesWidgetPtr = new QWavesWidget(ui.centralWidget);
	verticalLayout->addWidget(m_wavesWidgetPtr);

	//参数控件
	m_parameterListWidgetPtr = new QParameterListWidget(ui.centralWidget);
	verticalLayout->addWidget(m_parameterListWidgetPtr);

	//病人信息控件
	m_patientInfoWidgetPtr = new QPatientInfoWidget(ui.centralWidget);
	verticalLayout->addWidget(m_patientInfoWidgetPtr);

	//设置为滚动视图
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(ui.centralWidget);
	setCentralWidget(scrollArea);
}


/*!
    隐藏波形控件和参数控件
    @return  void
*/
void QExportXmlViewer::HideAllWidget()
{
	m_wavesWidgetPtr->setVisible(false);
	m_parameterListWidgetPtr->setVisible(false);
	m_patientInfoWidgetPtr->setVisible(false);
}


/*!
    “打开XML文件”响应函数
    @return  void
*/
void QExportXmlViewer::OnOpenClicked()
{
	//选择XML文件
	QVariant vVal;
	m_pCfgMgr->Instance()->GetConfig(ID_LAST_OPEN_FILE_PATH, vVal);
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open XML file"), vVal.toString(),
		tr("XML file (*.xml)"));
	if (!fileName.isEmpty())
	{
		//更新标题栏
		QString windowsTitle = QString(tr("%1 - %2")).arg(fileName).arg(EXPORT_XML_VIEWER); 
		setWindowTitle(windowsTitle);

		//保存路径
		QString filePath = QFileInfo(fileName).path();
		m_pCfgMgr->Instance()->UpdateConfig(ID_LAST_OPEN_FILE_PATH, filePath);

		//解析文件
		if (NULL == m_parserPtr)
		{
			m_parserPtr = new QExportXmlParser(this);
		}
		QString errorMsg;
		bool bSuccess = m_parserPtr->ParseFile(fileName, errorMsg);
		if (!bSuccess)
		{
			HideAllWidget();
			OnWarningInformation(QString(tr("Parse XML file errorMsg: %1").arg(errorMsg)));
			return;
		}

		//默认显示波形
		ui.action_Draw_Waveforms->setChecked(true);
		RepaintWave();
	}
	else
	{
		return;
	}
}


/*!
    “退出”响应函数
    @return  void
*/
void QExportXmlViewer::OnQuitClicked()
{
	QApplication::instance()->quit();
}


/*!
    “关于”响应函数
    @return  void
*/
void QExportXmlViewer::OnAboutClicked()
{
	QMessageBox::about(this, tr("About ExportXmlViewer"),
		tr("<h2>ExportXmlViewer 0.5.1</h2>"
		"<p>Copyright &copy; 2013-2015 Mindray."
		"<p>ExportXmlViewer is a small application that view XML files export by Central Monitoring System."
		"<p><h3>usage:</h3> "
		"<ul>"
		"<Li>'File | Open XML Files', open XML files</li>"
		"<p>"
		"<Li>'View | Show Wave Information', view <b>'MedianBeatTemplates'</b> information</li>"
		"<p>"
		"<Li>'View | Draw Waveforms', view <b>'Waveforms'</b> element</li>"
		"<p>"
		"<Li>'View | Draw MedianBeatTemplates', view <b>'MedianBeatTemplates'</b> element</li>"
		"<p>"
		"<Li>'View | List Observations', view <b>'Observations'</b> element</li>"
		"<p>"
		"<Li>'View | Patient Information', view <b>'Patient'</b> element</li>"
		));
}


/*!
    “显示波形信息”响应函数
    @return  void
*/
void QExportXmlViewer::OnShowWaveInfoClicked()
{
	if (NULL != m_parserPtr)
	{
		STUWaveform* pWavefrom = NULL;
		const QVector<STUWaveform*>& vecWaveforms = m_parserPtr->GetMedianBeatTemplates();

		if (vecWaveforms.size() > 0)
		{
			pWavefrom = vecWaveforms[0];

			QWaveInfoDlg dlg(this);
			dlg.setWindowTitle(MEDIANBEATTEMPLATES_INFORMATION);
			if (NULL!=pWavefrom && pWavefrom->m_waveformSegments.size()>0)
			{
				STUWaveformSegment* pSeg = pWavefrom->m_waveformSegments[0];
				dlg.SetTime(pSeg->m_time);
				dlg.SetSampleRate(pSeg->m_sampleRate);
				dlg.SetDataResolution(pSeg->m_dataResolation);
				dlg.SetUnits(pWavefrom->m_units);
				dlg.SetHighPass(pWavefrom->m_filters.m_highPass);
				dlg.SetLowPass(pWavefrom->m_filters.m_lowPass);
				dlg.SetBaselineDriftRemovel(pWavefrom->m_filters.m_baselineDriftRemoval);
			}
			dlg.exec();
		}
		else
		{
			QString strError = QString(tr("%1 is Empty.")).arg(MEDIANBEATTEMPLATES_INFORMATION);
			OnWarningInformation(strError);
		}
	}
	else
	{
		OnWarningInformation(tr("Please Open Xml files ..."));//提示打开文件
	}
}


/*!
    显示（波形、平均模板、参数列表）
    @return  void
*/
void QExportXmlViewer::OnActionGroupTriggered(QAction* action)
{
	if (NULL != m_parserPtr)
	{
		OnWarningInformation("");//清除错误信息
	}
	else
	{
		OnWarningInformation(tr("Please Open Xml files ..."));//提示打开文件
	}

	if (action == ui.action_Draw_Waveforms)
	{
		OnDrawWaveformsClicked();
	}
	else if (action == ui.action_Draw_MedianBeatTemplates)
	{
		OnDrawMedianBeatTemplatesClicked();
	}
	else if (action == ui.action_List_Observations)
	{
		OnListObservationsClicked();
	}
	else if (action == ui.action_Patient_Information)
	{
		OnPatientInformationClicked();
	}
	else
	{
		Q_ASSERT(false);
	}
}


/*!
    “绘制波形”响应函数
    @return  void
*/
void QExportXmlViewer::OnDrawWaveformsClicked()
{
	if (NULL != m_parserPtr)
	{
		HideAllWidget();//防止占用多余的空间.先隐藏所有控件
		const QVector<STUWaveform*>& vecWaveforms = m_parserPtr->GetWaveforms();
		m_wavesWidgetPtr->DrawWaveforms(vecWaveforms);
		m_wavesWidgetPtr->setVisible(true);
	}
}


/*!
    “绘制平均模板”响应函数
    @return  void
*/
void QExportXmlViewer::OnDrawMedianBeatTemplatesClicked()
{
	if (NULL != m_parserPtr)
	{
		HideAllWidget();//防止占用多余的空间.先隐藏所有控件
		const QVector<STUWaveform*>& vecWaveforms = m_parserPtr->GetMedianBeatTemplates();
		m_wavesWidgetPtr->DrawWaveforms(vecWaveforms);
		m_wavesWidgetPtr->setVisible(true);
	}
}


/*!
    显示参数列表
    @return  void
*/
void QExportXmlViewer::OnListObservationsClicked()
{
	if (NULL != m_parserPtr)
	{
		const QVector<STUObservation*>& observations = m_parserPtr->GetObservations();
		const qint32 observationCount = observations.size();
		if (observationCount <= 0)
		{
			OnWarningInformation(QString(tr("No Parameter(Observation) Element.")));
		}

		HideAllWidget();//防止占用多余的空间.先隐藏所有控件

		m_parameterListWidgetPtr->FillObservations(observations);

		m_parameterListWidgetPtr->setVisible(true);
	}
}


/*!
    显示病人信息
    @return  void
*/
void QExportXmlViewer::OnPatientInformationClicked()
{
	if (NULL != m_parserPtr)
	{
		const STUPatient * patientPtr = m_parserPtr->GetPatient();
		if (NULL != patientPtr)
		{
			HideAllWidget();//防止占用多余的空间.先隐藏所有控件

			m_patientInfoWidgetPtr->FillPatient(patientPtr);

			m_patientInfoWidgetPtr->setVisible(true);
		}
		else
		{
			OnWarningInformation(QString(tr("No Patient Element.")));
		}
	}
}

/*!
    “波形设置”响应函数
    @return  void
*/
void QExportXmlViewer::OnDisplaySettingsClicked()
{
	QDisplaySettingDlg dlg(this);
	connect(&dlg, SIGNAL(IntelligentCalculateScale(bool)), this, SLOT(OnIntelligentCalculateScaleChanged(bool)));
	connect(&dlg, SIGNAL(DrawMedicalECGWave(bool)), this, SLOT(OnDrawMedicalECGWaveChanged(bool)));
	connect(&dlg, SIGNAL(DrawWaveScaleText(bool)), this, SLOT(OnDrawWaveScaleTextChanged(bool)));
	dlg.exec();
}


/*!
    信息提示槽函数。调用场景：
	1. 当解析出错时调用；
	2. 当波形条数超过界面最大支持数目时调用；
	3. 超过当前界面最大绘制的秒数；
	4. 也可以手动触发
    @para[in]  const QString & info  
    @return  void
*/
void QExportXmlViewer::OnWarningInformation(const QString& info)
{
	if (info.length()>0)
	{
		m_infoLabelPtr->setText( QString(tr(" *** WARNING *** ")) + info);
	}
	else
	{
		m_infoLabelPtr->setText(info);
	}
}


/*!
    当用户在设置界面更改智能标尺设置时，调用此函数，重新刷新波形。
    @para[in]  bool  
    @return  void
*/
void QExportXmlViewer::OnIntelligentCalculateScaleChanged(bool /*bIntelligentCalculateScale*/)
{
	RepaintWave();
}


/*!
    重刷波形
    @para[in]  bool  
    @para[out]  bool  
    @return  void
*/
void QExportXmlViewer::OnDrawMedicalECGWaveChanged(bool /*bDrawMedicalECGWave*/)
{
	RepaintWave();
}


/*!
    本处无须重绘波形。为了简单起见，直接重给波形区
    @para[in]  bool bWaveScaleText  
    @return  void
*/
void QExportXmlViewer::OnDrawWaveScaleTextChanged(bool /*bWaveScaleText*/)
{
	RepaintWave();
}


/*!
    重刷波形
    @return  void
*/
void QExportXmlViewer::RepaintWave()
{
	if (ui.action_Draw_Waveforms->isChecked())
	{
		OnDrawWaveformsClicked();
	}
	else if (ui.action_Draw_MedianBeatTemplates->isChecked())
	{
		OnDrawMedianBeatTemplatesClicked();
	}
}

/*!
    更新配置值,需要放在InitDialog的前面
*/
void QExportXmlViewer::UpdateDisplayPixelsPerCM()
{
    qreal horDotsPerCM = 44;
    qreal verDotsPerCM = 44;

#ifdef Q_OS_WIN
    EDIDReader reader;
    QString strManufacture = reader.GetManufactureName();
    horDotsPerCM = 10.0 * reader.GetScreenWidth() / reader.GetScreenWidthMM();
    verDotsPerCM = 10.0 * reader.GetScreenHeight() / reader.GetScreenHeightMM();

    if (reader.GetScreenWidthMM() == 0 || reader.GetScreenHeightMM() == 0)
    {
        QMessageBox msgBox(QMessageBox::Warning, 
            "Display Information", 
            "Read display information failed",
            QMessageBox::Ok, 
            this);
        msgBox.exec();
    }
    else
    {
        horDotsPerCM = 10.0 * reader.GetScreenWidth() / reader.GetScreenWidthMM();
        verDotsPerCM = 10.0 * reader.GetScreenHeight() / reader.GetScreenHeightMM();

        QString msgBoxTitle = tr("Display Information");
        QString msgBoxText;

        if (reader.HasMultiScreen())
        {
            msgBoxText = QString("This computer has multi-screen. For 1mV ECG wave draw 1cm.\n Please use this display:\n\n\tManufacture: %1\n\tSceen size: %2mm * %3mm\n\tResolution: %4 * %5")
                .arg(strManufacture)
                .arg(reader.GetScreenWidthMM())
                .arg(reader.GetScreenHeightMM())
                .arg(reader.GetScreenWidth())
                .arg(reader.GetScreenHeight());
        }
        else
        {
            msgBoxText = QString("This PC has only one screen. Display information:\n\n\tManufacture: %1\n\tSceen size: %2mm * %3mm\n\tResolution: %4 * %5")
                .arg(strManufacture)
                .arg(reader.GetScreenWidthMM())
                .arg(reader.GetScreenHeightMM())
                .arg(reader.GetScreenWidth())
                .arg(reader.GetScreenHeight());
        }

        QMessageBox msgBox(QMessageBox::Information, msgBoxTitle, msgBoxText, QMessageBox::Ok, this);
        msgBox.exec();
    }
#else
	horDotsPerCM = width();
	horDotsPerCM = 10.0 * horDotsPerCM / widthMM();

    verDotsPerCM = height();
    verDotsPerCM = 10.0 * VerDotsPerCM / heightMM();
#endif

    //更新配置值,需要放在InitDialog的前面
    m_pCfgMgr->UpdateConfig(ID_HOR_PIXELS_PER_CM, horDotsPerCM);
    m_pCfgMgr->UpdateConfig(ID_VER_PIXELS_PER_CM, verDotsPerCM);
}