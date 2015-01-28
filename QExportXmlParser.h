#ifndef QEXPORTXMLPARSER_H
#define QEXPORTXMLPARSER_H

struct STUFilters;
struct STUWaveformSegment;
struct STUWaveform;
struct STUAnnotation;
struct STUObservation;

struct STUDemographics;
struct STUAssignedLocation;
struct STUDoctor;
struct STUTechnician;
struct STUMedications;
struct STUClinicalClassifications;
struct STUPatient;

#include <QObject>
#include <QDomNode>
#include <QVector>

/*
	完成波形节点XML的解析，并向外提供接口获取解析完的波形数据
	注意：
	节点的元素的处理，是大小写敏感的。大小写必须和qexportxmltagname.h中定义的节点名称一致。否则获取到空字串。
	为避免这种问题，元素值的获取忽略了大小写的问题，但是属性的获取还是大小写敏感
*/
class QExportXmlParser : public QObject
{
	Q_OBJECT

public:
	QExportXmlParser(QObject *parent);
	~QExportXmlParser();

public:
	bool ParseFile(const QString & strFileFullPath,  QString & errorMsg /*= QString("")*/);
	const QVector<STUWaveform*>& GetWaveforms() const;
	const QVector<STUWaveform*>& GetMedianBeatTemplates() const;
	const QVector<STUObservation*>& GetObservations() const;
	const STUPatient* GetPatient() const;

private:
	bool ParseWaveformNode(const QDomNode& waveformNode, STUWaveform* pWaveForm);
	bool ParseFiltersNode(const QDomNode& filterNode, STUFilters* pFilters);
	bool ParseWaveformSegmentNode(const QDomNode& waveformSegNode, STUWaveformSegment* pSeg);
	bool ParseAnnotationNode(const QDomNode& annotationNode, STUAnnotation* pAnnotation);
	bool ParseObservationNode(const QDomNode& observationNode, STUObservation* pObservation);
	//patient
	bool ParsePatientNode(const QDomNode& patientNode, STUPatient* pPatient);
	bool ParseDemographicsNode(const QDomNode& demographicsNode, STUDemographics& demographics);
	bool ParseAssignedLocationNode(const QDomNode& assignedLocationNode, STUAssignedLocation& assignedLocation);
	bool ParseDoctorNode(const QDomNode& doctorNode, STUDoctor& doctor);
	bool ParseTechnicianNode(const QDomNode& technicianNode, STUTechnician& technician);
	bool ParseMedicationsNode(const QDomNode& medicationsNode, STUMedications& medications);
	bool ParseClinicalClassificationsNode(const QDomNode& clinicalClassificationsNode, STUClinicalClassifications& clinicalClassifications);

private:
	void Init();
	void Clear();
	template <typename T> void Clear(QVector<T*> & vecItems);
	bool IsEqual(const QString& l, const QString& r, Qt::CaseSensitivity cs=Qt::CaseInsensitive);
	void UnknownNodeFilter(const QDomNode& n);

private:
	QVector<STUWaveform*> m_waveforms;
	QVector<STUWaveform*> m_medianBeatTemp;
	QVector<STUObservation*> m_observations;
	struct STUPatient* m_patientPtr;
	
private:
	QString m_unknownTags;

private:
	Q_DISABLE_COPY(QExportXmlParser);
};

#endif // QEXPORTXMLPARSER_H
