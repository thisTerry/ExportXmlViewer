#include "QParameterListWidget.h"
#include "qexportxmlstruct.h"
#include <QHeaderView>

//列信息结构
struct STUColumnHeader
{
	QString columnName;//列名称
	qint32  columnWidth;//列宽
};
static STUColumnHeader stuColumnHeaderTable[] = {
	{ QString("Parameter"),	200 },
	{ QString("Channel"),	100 },
	{ QString("Lead"),		80 },
	{ QString("Location"),	150 },
	{ QString("Value"),		100 },
	{ QString("Units"),		80 },
};


/*!
    构造函数
    @para[in]  QWidget * parent  
    @return  
*/
QParameterListWidget::QParameterListWidget(QWidget *parent)
	: QTableWidget(parent)
{
	InitComponent();
}


/*!
    析构函数
    @return  
*/
QParameterListWidget::~QParameterListWidget()
{

}


/*!
    初始化控件的显示
    @return  void
*/
void QParameterListWidget::InitComponent()
{
	//列标题
	const qint32 columnCount = sizeof(stuColumnHeaderTable)/sizeof(stuColumnHeaderTable[0]);
	setColumnCount(columnCount);

	for (qint32 i=0; i<columnCount; i++)
	{
		QTableWidgetItem* headerItem = new QTableWidgetItem();
		headerItem->setText(stuColumnHeaderTable[i].columnName);
		setHorizontalHeaderItem(i, headerItem);
		setColumnWidth(i, stuColumnHeaderTable[i].columnWidth);
	}

	//属性设置
	//horizontalHeader()->setResizeMode(QHeaderView::Stretch);//占满控件的宽度
	setSortingEnabled(false);//禁止排序
	setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去掉水平滚动条
	setSelectionBehavior(QAbstractItemView::SelectRows);//一次选中一行
	setSelectionMode(QAbstractItemView::ExtendedSelection);//可以拖动区域，选中多行
	//设置行和列的大小设为与内容相匹配
	//resizeColumnsToContents();
	//resizeRowsToContents();

	setFixedSize(sizeHint());
}


/*!
    显示参数
    @para[in]  const QVector<STUObservation * > & observations  
    @return  void
*/
void QParameterListWidget::FillObservations(const QVector<STUObservation*>& observations )
{
	const qint32 observationCount = observations.size();
	//清空内容
	clearContents();
	setRowCount(observationCount);
	//移除多余的行
	for (qint32 i=observationCount; i<rowCount(); i++)
	{
		removeRow(i);
	}

	//填充内容
	QTableWidgetItem* tableWidgetItem = NULL;
	for (qint32 i=0; i<observationCount; i++)
	{
		int columnIndex = 0;
		//Type
		tableWidgetItem = new QTableWidgetItem(observations[i]->m_type);
		setItem(i, columnIndex, tableWidgetItem);
		item(i, columnIndex)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		columnIndex++;

		//Channel
		tableWidgetItem = new QTableWidgetItem(observations[i]->m_channel);
		setItem(i, columnIndex, tableWidgetItem);
		item(i, columnIndex)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		columnIndex++;

		//Lead
		tableWidgetItem = new QTableWidgetItem(observations[i]->m_lead);
		setItem(i, columnIndex, tableWidgetItem);
		item(i, columnIndex)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		columnIndex++;

		//Location
		tableWidgetItem = new QTableWidgetItem(observations[i]->m_location);
		setItem(i, columnIndex, tableWidgetItem);
		item(i, columnIndex)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		columnIndex++;

		//Value
		tableWidgetItem = new QTableWidgetItem(observations[i]->m_value);
		setItem(i, columnIndex, tableWidgetItem);
		item(i, columnIndex)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		columnIndex++;

		//Units
		tableWidgetItem = new QTableWidgetItem(observations[i]->m_units);
		setItem(i, columnIndex, tableWidgetItem);
		item(i, columnIndex)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		columnIndex++;
	}
}


/*!
    IMPORTANT！！
	重写此函数，用于调整控件的初始大小。
    @return  QSize
*/
QSize QParameterListWidget::sizeHint() const
{
	const qint32 iRowCount = rowCount();
	if (iRowCount > 0)
	{
		//高度
		qint32 iHeight = horizontalHeader()->height()
			+ iRowCount * rowHeight(0)
			+ 2; //不加2会显示滚动条

		//宽度
		qint32 iWidth = verticalHeader()->width() + 2;
		const qint32 columnCount = sizeof(stuColumnHeaderTable)/sizeof(stuColumnHeaderTable[0]);
		for (qint32 i=0; i<columnCount; i++)
		{
			iWidth += stuColumnHeaderTable[i].columnWidth;
		}
		return QSize(iWidth, iHeight);
	}
	return QSize(0, 0);
}


/*!
    重载基类函数
    @para[in]  bool visible  
    @return  void
*/
void QParameterListWidget::setVisible(bool visible)
{
	QTableWidget::setVisible(visible);
	if (visible)
	{
		setFixedSize(sizeHint());
	}
	else
	{
		setFixedSize(0,0);
	}
}