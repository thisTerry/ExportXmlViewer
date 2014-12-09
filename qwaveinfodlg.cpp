#include "qwaveinfodlg.h"
#include <QHeaderView>

//列标题
static const QString COLUMN_NAME  = QString("Name");
static const QString COLUMN_VALUE = QString("Value");

//行内容
static const QString ROW_TIME           = QString("Time");
static const QString ROW_SAMPLE_RATE    = QString("Sample Rate");
static const QString ROW_DATARESOLUTION = QString("Data Resolution");
static const QString ROW_UNITS          = QString("Units");
static const QString ROW_HIGHPASS       = QString("High Pass");
static const QString ROW_LOWPASS        = QString("Low Pass");
static const QString ROW_BASELINE_DRIFT_REMOVAL = QString("Baseline Drift Removal");


QWaveInfoDlg::QWaveInfoDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_stringList.append(ROW_TIME);
	m_stringList.append(ROW_SAMPLE_RATE);
	m_stringList.append(ROW_DATARESOLUTION);
	m_stringList.append(ROW_UNITS);
	m_stringList.append(ROW_HIGHPASS);
	m_stringList.append(ROW_LOWPASS);
	m_stringList.append(ROW_BASELINE_DRIFT_REMOVAL);

	OnInitDialog();
}

QWaveInfoDlg::~QWaveInfoDlg()
{

}

void QWaveInfoDlg::OnInitDialog()
{
	//属性设置
	ui.tableWidget->setSortingEnabled(false);//禁止排序
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//一次选中一行
	ui.tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//可以拖动区域，选中多行
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	//列标题
	if (ui.tableWidget->columnCount() < 2)
	{
		ui.tableWidget->setColumnCount(2);
	}
	QTableWidgetItem* headerItem = new QTableWidgetItem();
	headerItem->setText(COLUMN_NAME);
	ui.tableWidget->setHorizontalHeaderItem(0, headerItem);
	headerItem = new QTableWidgetItem();
	headerItem->setText(COLUMN_VALUE);
	ui.tableWidget->setHorizontalHeaderItem(1, headerItem);

	//行内容
	if (ui.tableWidget->rowCount() < m_stringList.size())
	{
		ui.tableWidget->setRowCount(m_stringList.size());
	}
	for (qint32 i=0; i<m_stringList.size(); i++)
	{
		QTableWidgetItem* tableWidgetItem = new QTableWidgetItem(m_stringList.value(i));
		ui.tableWidget->setItem(i, 0, tableWidgetItem);
		ui.tableWidget->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		//无效值
		tableWidgetItem = new QTableWidgetItem(tr("---"));
		ui.tableWidget->setItem(i, 1, tableWidgetItem);
		ui.tableWidget->item(i, 1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	}

	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(OnCloseClicked()));
}

void QWaveInfoDlg::SetTime(const QString & time)
{
	SetItemText(ROW_TIME, time);
}

void QWaveInfoDlg::SetSampleRate(const QString & sampleRate)
{
	SetItemText(ROW_SAMPLE_RATE, sampleRate);
}

void QWaveInfoDlg::SetDataResolution(const QString & dataResolution)
{
	SetItemText(ROW_DATARESOLUTION, dataResolution);
}

void QWaveInfoDlg::SetUnits(const QString & units)
{
	SetItemText(ROW_UNITS, units);
}

void QWaveInfoDlg::SetHighPass(const QString & hightPass)
{
	SetItemText(ROW_HIGHPASS, hightPass);
}

void QWaveInfoDlg::SetLowPass(const QString & lowPass)
{
	SetItemText(ROW_LOWPASS, lowPass);
}

void QWaveInfoDlg::SetBaselineDriftRemovel(const QString & baselineDriftRemoval)
{
	SetItemText(ROW_BASELINE_DRIFT_REMOVAL, baselineDriftRemoval);
}

void QWaveInfoDlg::SetItemText(const QString& name, const QString& value)
{
	QTableWidgetItem* itemPtr = ui.tableWidget->item(m_stringList.indexOf(name), 1);
	if (NULL != itemPtr)
	{
		itemPtr->setText(value);
	}
	else
	{
		Q_ASSERT(false);
	}
}

void QWaveInfoDlg::OnCloseClicked()
{
	close();
}
