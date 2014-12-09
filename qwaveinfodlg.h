#ifndef QWAVEINFODLG_H
#define QWAVEINFODLG_H

#include <QDialog>
#include "ui_qwaveinfodlg.h"
#include <QStringList>

/*
波形信息窗口。
*/
class QWaveInfoDlg : public QDialog
{
	Q_OBJECT

public:
	QWaveInfoDlg(QWidget *parent = 0);
	~QWaveInfoDlg();

	void OnInitDialog();

	void SetTime(const QString & time);
	void SetSampleRate(const QString & sampleRate);
	void SetDataResolution(const QString & dataResolution);
	void SetUnits(const QString & units);
	void SetHighPass(const QString & hightPass);
	void SetLowPass(const QString & lowPass);
	void SetBaselineDriftRemovel(const QString & baselineDriftRemoval);

private:
	void SetItemText(const QString& name, const QString& value);

public slots:
	void OnCloseClicked();

private:
	QStringList m_stringList;

private:
	Ui::QWaveInfoDlg ui;
	Q_DISABLE_COPY(QWaveInfoDlg);
};

#endif // QWAVEINFODLG_H
