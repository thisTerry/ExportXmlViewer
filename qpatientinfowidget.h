#ifndef QPATIENTINFOWIDGET_H
#define QPATIENTINFOWIDGET_H

#include <QTreeWidget>
class QBrush;

struct STUDemographics;
struct STUAssignedLocation;
struct STUDoctor;
struct STUTechnician;
struct STUMedications;
struct STUClinicalClassifications;
struct STUPatient;

/*
	病人信息控件
*/
class QPatientInfoWidget : public QTreeWidget
{
	Q_OBJECT
public:
	QPatientInfoWidget(QWidget* parent);
	~QPatientInfoWidget();

	void FillPatient(const STUPatient* patientPtr);

public:
	virtual QSize sizeHint() const;
	virtual void setVisible(bool visible);

private:
	void InitComponent();

	void AddDemographics(const STUDemographics& demographics);
	void AddAssignedLocation(const STUAssignedLocation& assignedLocation);
	void AddDoctor(const STUDoctor& doctor);
	void AddTechnician(const STUTechnician& technician);
	void AddMedications(const STUMedications& medications);
	void AddClinicalClassifications(const STUClinicalClassifications& classifications);

private:
	QTreeWidgetItem* AddChildItem(QTreeWidget* parentItem, const QString& name);
	QTreeWidgetItem* AddChildItem(QTreeWidget* parentItem, const QString& name, const QString& value);
	QTreeWidgetItem* AddChildItem(QTreeWidgetItem* parentItem, const QString& name, const QString& value);

private:
	QBrush * m_bkgrdBrushPtr;

private:
	Q_DISABLE_COPY(QPatientInfoWidget);
};

#endif //QPATIENTINFOWIDGET_H