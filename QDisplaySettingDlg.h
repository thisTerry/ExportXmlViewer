#ifndef QDISPLAYSETTINGDLG_H
#define QDISPLAYSETTINGDLG_H

#include <QDialog>
#include "ui_qdisplaysettingdlg.h"

class CFGMgr;

class QDisplaySettingDlg : public QDialog
{
	Q_OBJECT

public:
	QDisplaySettingDlg(QWidget *parent = 0);
	~QDisplaySettingDlg();

	void InitDialog();

signals:
	void IntelligentCalculateScale(bool bIntelligentCalculateScale);
	void DrawMedicalECGWave(bool bDrawMedicalECGWaveChecked);
	void DrawWaveScaleText(bool bDrawWaveScaleTextChecked);

private slots:
	void OnIntelligentCalculateScaleChanged(int state);
	void OnHashNameCharCountChanged(int value);
	void OnDrawMedicalECGWaveChanged(int state);
	void OnDrawWaveScaleTextChanged(int state);

protected:
	virtual void closeEvent ( QCloseEvent * e );

private:
	CFGMgr* m_pCfgMgr;

private:
	Ui::QDisplaySettingDlg ui;
	Q_DISABLE_COPY(QDisplaySettingDlg);
};

#endif // QDISPLAYSETTINGDLG_H
