#include "QDisplaySettingDlg.h"
#include "CFGMgr.h"

const static QString WAVE_RENDER_SETTINGS = QString("Display Settings");

QDisplaySettingDlg::QDisplaySettingDlg(QWidget *parent)
	: QDialog(parent)
	, m_pCfgMgr(CFGMgr::Instance())
{
	ui.setupUi(this);
	InitDialog();
}

QDisplaySettingDlg::~QDisplaySettingDlg()
{

}

void QDisplaySettingDlg::InitDialog()
{
	setWindowTitle(WAVE_RENDER_SETTINGS);

	connect(ui.checkBoxIntelligentWaveScale, SIGNAL(stateChanged(int)), this, SLOT(OnIntelligentCalculateScaleChanged(int)));
	connect(ui.spinBoxHashNameCharCount, SIGNAL(valueChanged(int)), this, SLOT(OnHashNameCharCountChanged(int)));
	connect(ui.checkBoxDrawMedicalECGWave, SIGNAL(stateChanged(int)), this, SLOT(OnDrawMedicalECGWaveChanged(int)));
	connect(ui.checkBoxDrawWaveScaleText, SIGNAL(stateChanged(int)), this, SLOT(OnDrawWaveScaleTextChanged(int)));

	
	QVariant vVal;
	m_pCfgMgr->GetConfig(ID_MAX_RENDER_SECONDS, vVal);
	ui.spinBoxMaxRenderSecs->setValue(vVal.toInt());

	bool bIntelligentWaveScaleChecked = false;
    m_pCfgMgr->GetConfig(ID_INTELIIGENT_CALCULATE_SCALE, vVal);
	bIntelligentWaveScaleChecked = vVal.toBool();
    ui.checkBoxIntelligentWaveScale->setChecked(bIntelligentWaveScaleChecked);
	
	m_pCfgMgr->GetConfig(ID_HASH_NAME_CHAR_COUNT, vVal);
	ui.spinBoxHashNameCharCount->setValue(vVal.toInt());
	ui.spinBoxHashNameCharCount->setEnabled(bIntelligentWaveScaleChecked);

	bool bDrawMedicalECGWaveChecked = false;
	m_pCfgMgr->GetConfig(ID_RENDER_MEDICAL_ECG_WAVE, vVal);
	bDrawMedicalECGWaveChecked = vVal.toBool();
	ui.checkBoxDrawMedicalECGWave->setChecked(bDrawMedicalECGWaveChecked);

	bool bDrawWaveScaleTextChecked = false;
	m_pCfgMgr->GetConfig(ID_RENDER_WAVE_SCALE_TEXT, vVal);
	bDrawWaveScaleTextChecked = vVal.toBool();
	ui.checkBoxDrawWaveScaleText->setChecked(bDrawWaveScaleTextChecked);
}

void QDisplaySettingDlg::OnIntelligentCalculateScaleChanged(int state)
{
	bool bChecked = (state == Qt::Checked) ? true : false;
	m_pCfgMgr->UpdateConfig(ID_INTELIIGENT_CALCULATE_SCALE, bChecked);

	ui.spinBoxHashNameCharCount->setEnabled(bChecked);

	emit IntelligentCalculateScale(bChecked);
}

void QDisplaySettingDlg::OnHashNameCharCountChanged(int value)
{
	m_pCfgMgr->UpdateConfig(ID_HASH_NAME_CHAR_COUNT, value);

	bool bChecked = ui.checkBoxIntelligentWaveScale->isChecked();
	emit IntelligentCalculateScale(bChecked);//刷新界面
}

void QDisplaySettingDlg::OnDrawMedicalECGWaveChanged(int state)
{
	bool bChecked = (state == Qt::Checked) ? true : false;
	m_pCfgMgr->UpdateConfig(ID_RENDER_MEDICAL_ECG_WAVE, bChecked);

	emit DrawMedicalECGWave(bChecked);
}

void QDisplaySettingDlg::OnDrawWaveScaleTextChanged(int state)
{
	bool bChecked = (state == Qt::Checked) ? true : false;
	m_pCfgMgr->UpdateConfig(ID_RENDER_WAVE_SCALE_TEXT, bChecked);

	emit DrawWaveScaleText(bChecked);
}

void QDisplaySettingDlg::closeEvent ( QCloseEvent * /*e*/ )
{
	m_pCfgMgr->UpdateConfig(ID_MAX_RENDER_SECONDS, ui.spinBoxMaxRenderSecs->value());
}
