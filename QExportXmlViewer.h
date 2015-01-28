#ifndef EXPORTXMLSHOW_H
#define EXPORTXMLSHOW_H

#include <QtWidgets/QMainWindow>
#include "ui_qexportxmlviewer.h"

struct STUWaveform;
struct STUAnnotation;
class QExportXmlParser;
class CFGMgr;
class QAction;
class QLabel;
class QWavesWidget;
class QParameterListWidget;
class QPatientInfoWidget;

/*
程序的主类。
聚合配置管理、XML解析类、波形绘制窗口，并响应用户操作。
*/
class QExportXmlViewer : public QMainWindow
{
	Q_OBJECT

public:
	QExportXmlViewer(QWidget *parent = 0);
	~QExportXmlViewer();

	void OnInitDialog();

public slots:
	void OnOpenClicked();
	void OnQuitClicked();
	void OnAboutClicked();
	void OnShowWaveInfoClicked();
	void OnActionGroupTriggered(QAction* action);//显示（波形、平均模板、树视图）
	void OnDisplaySettingsClicked();
	void OnWarningInformation(const QString& info);
	void OnIntelligentCalculateScaleChanged(bool bIntelligentCalculateScale);
	void OnDrawMedicalECGWaveChanged(bool bDrawMedicalECGWave);
	void OnDrawWaveScaleTextChanged(bool bWaveScaleText);

private slots:
	void OnDrawWaveformsClicked();
	void OnDrawMedianBeatTemplatesClicked();
	void OnListObservationsClicked();
	void OnPatientInformationClicked();

private:
	void InitCentralWidget();
	void HideAllWidget();
	void RepaintWave();

private:
    void UpdateDisplayPixelsPerCM();
	qreal GetHorizontalPixelsPerCentimeter();
	qreal GetVerticalPixelsPerCentimeter();

private:
    QExportXmlParser*            m_parserPtr;//12导 XML 解析器
	QLabel*                      m_infoLabelPtr;
    QWavesWidget*                m_wavesWidgetPtr;
	QParameterListWidget*        m_parameterListWidgetPtr;//参数列表控件
	QPatientInfoWidget*          m_patientInfoWidgetPtr;

    CFGMgr*                      m_pCfgMgr;//配置管理器

private:
	Ui::ExportXmlViewerClass ui;
	Q_DISABLE_COPY(QExportXmlViewer);
};

#endif // EXPORTXMLSHOW_H
