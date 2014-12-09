#ifndef QWAVESWIDGET_H
#define QWAVESWIDGET_H

#include <QWidget>

class QLabel;
struct STUWaveform;
struct STUAnnotation;
namespace psimpl
{
	class RenderArea;
}

/*
	一组波形控件
	每一组波形：有一个波形名称label和一个波形区域RenderArea
*/
class QWavesWidget : public QWidget
{
	Q_OBJECT

public:
	QWavesWidget(QWidget* parent);
	~QWavesWidget();

	void DrawWaveforms(const QVector<STUWaveform*>& wavefroms);
	
signals:
	void WarningInformation(const QString& info);

public slots:
	void OnWarningInformation(const QString& info);

private:
	void InitComponent();

	QLabel* GetWaveNameLabel(qint32 iChannel) const;
	psimpl::RenderArea* GetRenderArea(qint32 iChannel) const;
	void FillWaveformData(qint32 iWaveformIndex, const STUWaveform* pWaveform);
	void ProcessIntelligentCalculateScale(qint32 iWaveCount);
	void AppendPacerPosition(const QVector<STUAnnotation*>& vecAnnotation, qint32 iOffset, QString& strPacerPosition);

	void HideAllWidget();
	void SetUsefulWidgetVisable();

public:
	virtual QSize sizeHint() const;
	virtual void setVisible(bool visible);

private:
	qint32                       m_iCurWaveCount;//当前波形个数
	QVector<QLabel*>             m_vecWaveNameLabel;//波形名称标签
	QVector<psimpl::RenderArea*> m_vecRenderArea;//波形绘制区域

private:
	Q_DISABLE_COPY(QWavesWidget);
};

#endif // QWAVESWIDGET_H
