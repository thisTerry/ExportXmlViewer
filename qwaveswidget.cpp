#include "qwaveswidget.h"
#include <QGridLayout>
#include <QLabel>
#include "RenderArea.h"
#include "scalecalculator.h"
#include "qexportxmlstruct.h"
#include "CFGMgr.h"

using namespace psimpl;

static const QString WAVEFORMS                       = QString("Waveforms");
static const QString MEDIANBEATTEMPLATES             = QString("MedianBeatTemplates");
static const qint32  MAX_SHOW_WAVE_COUNT             = 32;//最大支持显示的波形条数
static const qint32  LAYOUT_SPACING                  = 6;
static const qint32  LAYOUT_CONTENT_MARGINS          = 11;


QWavesWidget::QWavesWidget(QWidget* parent)
	: QWidget(parent)
	, m_iCurWaveCount(0)
{
	InitComponent();
}

QWavesWidget::~QWavesWidget()
{

}


/*!
    初始化控件的显示
    @return  void
*/
void QWavesWidget::InitComponent()
{
	QGridLayout* gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(LAYOUT_SPACING);
	gridLayout->setContentsMargins(11, 11, 11, 11);

	for (qint32 i=0; i<MAX_SHOW_WAVE_COUNT; i++)
	{
		//波形名称
		QLabel* label = new QLabel(this);
		gridLayout->addWidget(label, i, 0, 1, 1);
		m_vecWaveNameLabel.push_back(label);

		//波形绘制区域
		psimpl::RenderArea* renderArea = new psimpl::RenderArea(this);
		//QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
		//sizePolicy1.setHorizontalStretch(0);
		//sizePolicy1.setVerticalStretch(3);
		//sizePolicy1.setHeightForWidth(renderArea->sizePolicy().hasHeightForWidth());
		//renderArea->setSizePolicy(sizePolicy1);
		//renderArea->setMinimumSize(QSize(0, 0));
		//renderArea->setSizeIncrement(QSize(0, 0));
		//renderArea->setBaseSize(QSize(0, 0));
		renderArea->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
		//renderArea->setFrameShape(QFrame::StyledPanel);
		//renderArea->setFrameShadow(QFrame::Sunken);

		gridLayout->addWidget(renderArea, i, 1, 1, 1);
		m_vecRenderArea.push_back(renderArea);

		//信号连接
		connect(GetRenderArea(i), SIGNAL(WarningInformation(const QString&)), this, SLOT(OnWarningInformation(const QString&)));
	}

	setFixedSize(sizeHint());
}


/*!
    收到错误消息响应函数
    @para[in]  const QString & info  
	@return  void
*/
void QWavesWidget::OnWarningInformation(const QString& info)
{
	emit WarningInformation(info);
}


/*!
    根据位置索引，获取标签控件对象
    @para[in]  qint32 iChannel  
    @return  QLabel*
*/
QLabel* QWavesWidget::GetWaveNameLabel(qint32 iChannel) const
{
	if (iChannel>=0 && iChannel<m_vecWaveNameLabel.size())
	{
		return m_vecWaveNameLabel[iChannel];
	}
	return NULL;
}


/*!
    根据位置索引，获取波形绘制控件对象
    @para[in]  qint32 iChannel  
    @return  psimpl::RenderArea*
*/
psimpl::RenderArea* QWavesWidget::GetRenderArea(qint32 iChannel) const
{
	if (iChannel>=0 && iChannel<m_vecRenderArea.size())
	{
		return m_vecRenderArea[iChannel];
	}
	return NULL;
}


/*!
    在主界面绘制波形。
    @para[in]  const QVector<STUWaveform * > & wavefroms  
    @return  void
*/
void QWavesWidget::DrawWaveforms(const QVector<STUWaveform*>& wavefroms)
{
	const qint32 iXmlWaveCount = wavefroms.size();
	m_iCurWaveCount = (iXmlWaveCount < MAX_SHOW_WAVE_COUNT) ? iXmlWaveCount : MAX_SHOW_WAVE_COUNT;

	if (iXmlWaveCount > MAX_SHOW_WAVE_COUNT)
	{
		emit WarningInformation(QString(tr("Wave count exceed maximum support count : %1.")).arg(MAX_SHOW_WAVE_COUNT));
	}
	else if (0 == iXmlWaveCount)
	{
		emit WarningInformation(QString(tr("Not found %1 element or empty element.")).arg(WAVEFORMS));
	}

	for (qint32 i=0; i<m_iCurWaveCount; i++)
	{
		STUWaveform* pWaveform = wavefroms.at(i);
		FillWaveformData(i, pWaveform);
	}

	//自动波形标尺
	QVariant vVal;
	CFGMgr::Instance()->GetConfig(ID_INTELIIGENT_CALCULATE_SCALE, vVal);
	if (vVal.toBool())
	{
		ProcessIntelligentCalculateScale(m_iCurWaveCount);
	}

	//根据波形个数，调整尺寸
	setFixedSize(sizeHint());
}


/*!
    绘制波形函数。根据在指定索引的控件，绘制指令的波形
    @para[in]  qint32 iWaveformIndex  
    @para[in]  const STUWaveform * pWaveform  
    @return  void
*/
void QWavesWidget::FillWaveformData(qint32 iWaveformIndex, const STUWaveform* pWaveform)
{
	if (NULL!=pWaveform && iWaveformIndex>=0 && iWaveformIndex<MAX_SHOW_WAVE_COUNT)
	{
		GetWaveNameLabel(iWaveformIndex)->setText(pWaveform->m_type);//波形名称标签
		if (pWaveform->m_waveformSegments.size() > 0)
		{
			GetRenderArea(iWaveformIndex)->SetWaveName(pWaveform->m_type);//波形名称
			GetRenderArea(iWaveformIndex)->SetDataResolution(pWaveform->m_waveformSegments[0]->m_dataResolation);//分辨率
			GetRenderArea(iWaveformIndex)->SetUnits(pWaveform->m_units);//单位

			GetRenderArea(iWaveformIndex)->SetSampleRate(pWaveform->m_waveformSegments[0]->m_sampleRate);
			
			QString strAllWaveData;
			QString strPacerPosition;
			qint32 iSample = pWaveform->m_waveformSegments[0]->m_sampleRate.toInt();
			for (qint32 iSeg=0; iSeg<pWaveform->m_waveformSegments.size(); iSeg++)
			{
				strAllWaveData += pWaveform->m_waveformSegments[iSeg]->m_data;
				if (iSeg != (pWaveform->m_waveformSegments.size()-1))
				{
					strAllWaveData += QString(", ");
				}

				AppendPacerPosition(pWaveform->m_waveformSegments[iSeg]->m_annotation, iSeg*iSample, strPacerPosition);
			}
			GetRenderArea(iWaveformIndex)->SetWaveData(strAllWaveData);
			GetRenderArea(iWaveformIndex)->SetPacerPosition(strPacerPosition);

			//visible controls
			GetWaveNameLabel(iWaveformIndex)->setVisible(true);
			GetRenderArea(iWaveformIndex)->setVisible(true);
		}
	}
	else
	{
		Q_ASSERT(false);
	}
}


/*!
    生成PACE字符串。比如在分别在偏移位置100，200的地方绘制数据，
	则字符串为：“100，200，”。生成的字符串传递给绘制控件处理。
    @para[in]  const QVector<STUAnnotation * > & vecAnnotation  
    @para[in]  qint32 iOffset  累积偏移量
    @para[out]  QString & strPacerPosition  
    @return  void
*/
void QWavesWidget::AppendPacerPosition(const QVector<STUAnnotation*>& vecAnnotation, 
										qint32 iOffset, 
										QString& strPacerPosition)
{
	STUAnnotation* pAnnotation = NULL;

	for (QVector<STUAnnotation*>::const_iterator it=vecAnnotation.begin(); it!=vecAnnotation.end(); it++)
	{
		pAnnotation = *it;

		if (pAnnotation->m_type == QString("Pacer"))
		{
			qint32 iPacerPos = pAnnotation->m_offset.toInt() + iOffset;
			strPacerPosition += QString("%1,").arg(iPacerPos);
		}
	}
}


/*!
    处理智能标尺的绘制。
	即先从各控件中，取得最大值和最小值和波形名称。然后设置到标尺计算类，计算完成后，逐个控件刷新。
    @return  void
*/
void QWavesWidget::ProcessIntelligentCalculateScale(qint32 iWaveCount)
{
	ScaleCalculator scaleCalcualtor;
	qreal dbMinValue = 0;
	qreal dbMaxValue = 0;

	//获取当前所有显示波形的最大值，最小值及名称
	for (qint32 iWaveformIndex=0; iWaveformIndex<iWaveCount; iWaveformIndex++)
	{
		GetRenderArea(iWaveformIndex)->GetMinMaxValue(dbMinValue, dbMaxValue);
		scaleCalcualtor.SetWaveMinMaxValue(GetWaveNameLabel(iWaveformIndex)->text(), dbMinValue, dbMaxValue);
	}

	//获取计算完成后的最大值和最小值，逐个刷新显示。
	for (qint32 iWaveformIndex=0; iWaveformIndex<iWaveCount; iWaveformIndex++)
	{
		scaleCalcualtor.GetWaveMinMaxValue(GetWaveNameLabel(iWaveformIndex)->text(),
			dbMinValue, dbMaxValue, GetRenderArea(iWaveformIndex)->GetDataResolution());
		GetRenderArea(iWaveformIndex)->SetMinMaxValue(dbMinValue, dbMaxValue);
	}
}


/*!
    重载基类函数
    @return  QT_NAMESPACE::QSize
*/
QSize QWavesWidget::sizeHint() const
{
	const psimpl::RenderArea* renderAreaPtr = GetRenderArea(0);
	const QLabel* labelPtr = GetWaveNameLabel(0);
	qint32 labelHeight = labelPtr->height();
	qint32 renderAreaHeight = renderAreaPtr->sizeHint().height();
	qint32 maxHeight = (labelHeight > renderAreaHeight) ? labelHeight : renderAreaHeight;

	qint32 iHeight = m_iCurWaveCount * maxHeight 
		+ (m_iCurWaveCount -1) * LAYOUT_SPACING
		+ LAYOUT_CONTENT_MARGINS * 2;

	qint32 iWidth = labelPtr->sizeHint().width() 
		+ LAYOUT_SPACING 
		+ renderAreaPtr->sizeHint().width() 
		+ LAYOUT_SPACING + LAYOUT_SPACING * 2;

	return QSize(iWidth, iHeight);
}


/*!
    重载基本函数
    @para[in]  bool visible  
    @para[out]  bool visible  
    @return  void
*/
void QWavesWidget::setVisible(bool visible)
{
	QWidget::setVisible(visible);
	if (visible)
	{
		SetUsefulWidgetVisable();
	}
	else
	{
		HideAllWidget();
	}
}


/*!
    隐藏所有控件
    @return  void
*/
void QWavesWidget::HideAllWidget()
{
	for (qint32 i=0; i<MAX_SHOW_WAVE_COUNT; i++)
	{
		GetWaveNameLabel(i)->setVisible(false);
		GetRenderArea(i)->setVisible(false);
	}
	setFixedSize(0,0);
}


/*!
    显示有用的控件
    @return  void
*/
void QWavesWidget::SetUsefulWidgetVisable()
{
	for (qint32 i=0; i<m_iCurWaveCount; i++)
	{
		GetWaveNameLabel(i)->setVisible(true);
		GetRenderArea(i)->setVisible(true);
	}
	//setFixedSize(sizeHint());
}