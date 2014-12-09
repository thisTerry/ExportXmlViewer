#include "QPatientInfoWidget.h"
#include "qexportxmlstruct.h"
#include <QBrush>
#include "qexportxmltagname.h"

//列信息结构
struct STUColumnHeader
{
	QString columnName;//列名称
	qint32  columnWidth;//列宽
};
static STUColumnHeader stuColumnHeaderTable[] = {
	{ QString("Name"),	200 },
	{ QString("Value"),	250 },
};

/*!
    构造函数
    @para[in]  QWidget * parent  
    @return  
*/
QPatientInfoWidget::QPatientInfoWidget(QWidget *parent)
	: QTreeWidget(parent)
	, m_bkgrdBrushPtr(NULL)
{
	InitComponent();

	m_bkgrdBrushPtr = new QBrush(QColor(Qt::white));
	m_bkgrdBrushPtr->setStyle(Qt::SolidPattern);
}


/*!
    析构函数
    @return  
*/
QPatientInfoWidget::~QPatientInfoWidget()
{

}


/*!
    初始化控件的显示
    @return  void
*/
void QPatientInfoWidget::InitComponent()
{
	//列标题
	const qint32 columnCount = sizeof(stuColumnHeaderTable)/sizeof(stuColumnHeaderTable[0]);
	setColumnCount(columnCount);

	QStringList columnHeader;
	for (qint32 i=0; i<columnCount; i++)
	{
		columnHeader.append(stuColumnHeaderTable[i].columnName);
		setColumnWidth(i, stuColumnHeaderTable[i].columnWidth);
	}
	setHeaderLabels(columnHeader);
	
	setFixedSize(sizeHint());
}


/*!
    显示参数
    @para[in]  const STUPatient* patientPtr  
    @return  void
*/
void QPatientInfoWidget::FillPatient(const STUPatient* patientPtr)
{
	//清空内容
	clear();

	if (NULL != patientPtr)
	{
		//demographics
		AddDemographics(patientPtr->m_demographics);

		//assignedLocation
		AddAssignedLocation(patientPtr->m_assignedLocation);

		//doctor
		AddDoctor(patientPtr->m_doctor);

		//technician
		AddTechnician(patientPtr->m_technician);

		//PACE
		AddChildItem(this, TAGNAME_PACED, patientPtr->m_paced);

		//medications
		AddMedications(patientPtr->m_medications);

		//clinicalClassifications
		AddClinicalClassifications(patientPtr->m_clinicalClassfications);
	}

	//全部展开
	expandAll();
}

void QPatientInfoWidget::AddDemographics(const STUDemographics& demographics)
{
	//Demographics
	QTreeWidgetItem* topLevelItem = AddChildItem(this, TAGNAME_DEMOGRAPHICS);

	//FirstName
	AddChildItem(topLevelItem, TAGNAME_FIRSTNAME, demographics.m_firstName);

	//LastName
	AddChildItem(topLevelItem, TAGNAME_LASTNAME, demographics.m_lastName);

	//PatientID
	AddChildItem(topLevelItem, TAGNAME_PATIENTID, demographics.m_patientID);

	//DateOfBirth
	AddChildItem(topLevelItem, TAGNAME_DATEOFBIRTH, demographics.m_dateOfBirth);

	//Age
	AddChildItem(topLevelItem, TAGNAME_AGE, demographics.m_age);

	//Gender
	AddChildItem(topLevelItem, TAGNAME_GENDER, demographics.m_gender);
	
	//Race
	AddChildItem(topLevelItem, TAGNAME_RACE, demographics.m_race);
}

void QPatientInfoWidget::AddAssignedLocation(const STUAssignedLocation& assignedLocation)
{
	//assignedLocation
	QTreeWidgetItem* topLevelItem = AddChildItem(this, TAGNAME_ASSIGNEDLOCATION);

	//bed
	AddChildItem(topLevelItem, TAGNAME_BED, assignedLocation.m_bed);

	//Room
	AddChildItem(topLevelItem, TAGNAME_ROOM, assignedLocation.m_room);

	//pointOfCare
	AddChildItem(topLevelItem, TAGNAME_POINTOFCARE, assignedLocation.m_pointOfCare);

	//Facility
	AddChildItem(topLevelItem, TAGNAME_FACILITY, assignedLocation.m_facility);
}

void QPatientInfoWidget::AddDoctor(const STUDoctor& doctor)
{
	//doctor
	QTreeWidgetItem* topLevelItem = AddChildItem(this, TAGNAME_DOCTOR);

	//name
	AddChildItem(topLevelItem, TAGNAME_NAME, doctor.m_name);
}

void QPatientInfoWidget::AddTechnician(const STUTechnician& technician)
{
	//technician
	QTreeWidgetItem* topLevelItem = AddChildItem(this, TAGNAME_TECHNICIAN);

	//name
	AddChildItem(topLevelItem, TAGNAME_NAME, technician.m_name);
}

void QPatientInfoWidget::AddMedications(const STUMedications& medications)
{
	//medications
	QTreeWidgetItem* topLevelItem = AddChildItem(this, TAGNAME_MEDICATIONS);

	//medication
	for (qint32 i=0; i<medications.m_name.size(); i++)
	{
		AddChildItem(topLevelItem, TAGNAME_MEDICATION, medications.m_name.at(i));
	}
}

void QPatientInfoWidget::AddClinicalClassifications(const STUClinicalClassifications& classifications)
{
	//classifications
	QTreeWidgetItem* topLevelItem = AddChildItem(this, TAGNAME_CLINICALCLASSIFICATIONS);

	//classification
	for (qint32 i=0; i<classifications.m_classifications.size(); i++)
	{
		AddChildItem(topLevelItem, TAGNAME_CLINICALCLASSIFICATION, classifications.m_classifications.at(i));
	}
}


/*!
    添加一个根节点。根节点不含有值
    @para[in]  QTreeWidget * parentItem  treewidget本身
    @para[in]  const QString & name  节点的名称
    @return  QTreeWidgetItem*
*/
QTreeWidgetItem* QPatientInfoWidget::AddChildItem(QTreeWidget* parentItem, const QString& name)
{
	QStringList itemStrList;
	itemStrList << name;
	QTreeWidgetItem* childItem = new QTreeWidgetItem(parentItem, itemStrList);

	if (NULL != m_bkgrdBrushPtr)
	{//背景色设置
		m_bkgrdBrushPtr->setColor(QColor(Qt::gray));
		childItem->setBackground(0, *m_bkgrdBrushPtr);//name column
		childItem->setBackground(1, *m_bkgrdBrushPtr);//value column
	}

	addTopLevelItem(childItem);
	return childItem;
}


/*!
    添加一个根节点。根节点含有值
    @para[in]  QTreeWidget * parentItem  treewidget本身
    @para[in]  const QString & name  节点名称
    @para[in]  const QString & value  节点值
    @return  QTreeWidgetItem*
*/
QTreeWidgetItem* QPatientInfoWidget::AddChildItem(QTreeWidget* parentItem, const QString& name, const QString& value)
{
	QStringList itemStrList;
	itemStrList << name << value;
	QTreeWidgetItem* childItem = new QTreeWidgetItem(parentItem, itemStrList);

	if (NULL != m_bkgrdBrushPtr)
	{//背景色设置
		m_bkgrdBrushPtr->setColor(QColor(Qt::gray));
		childItem->setBackground(0, *m_bkgrdBrushPtr);//name column
		if (value.isEmpty())
		{
			m_bkgrdBrushPtr->setColor(QColor(Qt::yellow));
		}
		childItem->setBackground(1, *m_bkgrdBrushPtr);//value column
	}

	addTopLevelItem(childItem);
	return childItem;
}


/*!
    添加一个子节点。
    @para[in]  QTreeWidgetItem * parentItem  待添加节点的父节点
    @para[in]  const QString & name  节点名称
    @para[in]  const QString & value  节点值
    @return  QTreeWidgetItem*
*/
QTreeWidgetItem* QPatientInfoWidget::AddChildItem(QTreeWidgetItem* parentItem, const QString& name, const QString& value)
{
	QStringList itemStrList;
	itemStrList << name << value;
	QTreeWidgetItem* childItem = new QTreeWidgetItem(parentItem, itemStrList);

	if (NULL != m_bkgrdBrushPtr)
	{//背景色设置
		m_bkgrdBrushPtr->setColor(QColor(Qt::cyan));
		childItem->setBackground(0, *m_bkgrdBrushPtr);//name column
		if (value.isEmpty())
		{
			m_bkgrdBrushPtr->setColor(QColor(Qt::yellow));
		}
		childItem->setBackground(1, *m_bkgrdBrushPtr);//value column
	}

	parentItem->addChild(childItem);
	return childItem;
}


/*!
    IMPORTANT！！
	重写此函数，用于调整控件的初始大小。
    @return  QSize
*/
QSize QPatientInfoWidget::sizeHint() const
{
	//高度
	qint32 iHeight = 600;

	//宽度
	qint32 iWidth = 0;
	const qint32 columnCount = sizeof(stuColumnHeaderTable)/sizeof(stuColumnHeaderTable[0]);
	for (qint32 i=0; i<columnCount; i++)
	{
		iWidth += stuColumnHeaderTable[i].columnWidth;
	}
	iWidth += 2;

	return QSize(iWidth, iHeight);
}


/*!
    重载基类函数
    @para[in]  bool visible  
    @return  void
*/
void QPatientInfoWidget::setVisible(bool visible)
{
	QTreeWidget::setVisible(visible);
	if (visible)
	{
		setFixedSize(sizeHint());
	}
	else
	{
		setFixedSize(0,0);
	}
}
