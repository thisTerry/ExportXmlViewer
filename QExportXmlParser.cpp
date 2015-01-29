#include "QExportXmlParser.h"
#include <QDomDocument>
#include <QFile>
#include <QtDebug>
#include "qexportxmlstruct.h"
#include "qexportxmltagname.h"


QExportXmlParser::QExportXmlParser(QObject *parent)
: QObject(parent)
, m_patientPtr(NULL)
{
	Init();
}

QExportXmlParser::~QExportXmlParser()
{
	Clear();
}

void QExportXmlParser::Init()
{

}

void QExportXmlParser::Clear()
{
	Clear(m_waveforms);
	Clear(m_medianBeatTemp);
	Clear(m_observations);
	if (NULL != m_patientPtr)
	{
		delete m_patientPtr;
		m_patientPtr = NULL;
	}
	m_unknownTags.clear();
}

template <typename T> void QExportXmlParser::Clear(QVector<T*> & vecItems)
{
	for (typename QVector<T*>::iterator it=vecItems.begin(); it!=vecItems.end(); it++)
	{
		T* p = *it;
		if (NULL != p)
		{
			delete p;
			p = NULL;
		}
	}
	vecItems.clear();
}

/*!
    文件解析
    @para[in]  const QString & strFileFullPath  
    @para[out]  QString & errorMsg  
    @return  bool
*/
bool QExportXmlParser::ParseFile(const QString & strFileFullPath, QString & errorMsg)
{
	Clear();
	errorMsg = tr("");

	QDomDocument doc;
	QFile file(strFileFullPath);  
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug("file.open failed.");
		errorMsg = tr("file.open failed.");
		return false;
	}
	qint32 errorLine = 0;
	qint32 errorColumn = 0;
	if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn))
	{
		qDebug("doc.setContent failed. errorMsg: %s, errorLine: %d, errorColumn: %d.\n", qPrintable(errorMsg), errorLine, errorColumn);
		errorMsg.append(QString(tr(", errorLine: %1, errorColumn: %2.")).arg(errorLine).arg(errorColumn));
		file.close();
		return false;
	}
	file.close();

	//waveforms
	QDomNodeList nodeList = doc.elementsByTagName(TAGNAME_WAVEFORMS);
	for (qint32 i=0; i<nodeList.count(); i++)
	{
		QDomElement e = nodeList.at(i).toElement();
		QDomNode n = e.firstChild();
		while(!n.isNull())
		{
			STUWaveform* pWaveForm = new STUWaveform;
			ParseWaveformNode(n, pWaveForm);
			m_waveforms.push_back(pWaveForm);
			n = n.nextSibling();
		}
	}

	//medianBeatTemplates
	nodeList = doc.elementsByTagName(TAGNAME_MEDIANBEATTEMPLATES);
	for (qint32 i=0; i<nodeList.count(); i++)
	{
		QDomElement e = nodeList.at(i).toElement();
		QDomNode n = e.firstChild();
		while(!n.isNull())
		{
			STUWaveform* pWaveForm = new STUWaveform;
			ParseWaveformNode(n, pWaveForm);
			m_medianBeatTemp.push_back(pWaveForm);
			n = n.nextSibling();
		}
	}

	//observations
	nodeList = doc.elementsByTagName(TAGNAME_OBSERVATIONS);
	for (qint32 i=0; i<nodeList.count(); i++)
	{
		QDomElement e = nodeList.at(i).toElement();
		QDomNode n = e.firstChild();
		while(!n.isNull())
		{
			STUObservation* pObservation = new STUObservation;
			ParseObservationNode(n, pObservation);
			m_observations.push_back(pObservation);
			n = n.nextSibling();
		}
	}

	//Patient
	nodeList = doc.elementsByTagName(TAGNAME_PATIENT);
	if (nodeList.count() > 0)
	{
		m_patientPtr = new STUPatient;
		ParsePatientNode(nodeList.at(0), m_patientPtr);
	}

	if (!m_unknownTags.isEmpty())
	{
		errorMsg = tr("Unknown Tags: %1").arg(m_unknownTags);
		return false;
	}

	return true;
}

bool QExportXmlParser::ParseWaveformNode(const QDomNode& waveformNode, STUWaveform* pWaveForm)
{
	bool bRet = true;
	if (NULL != pWaveForm)
	{
		QDomElement e = waveformNode.toElement();
		if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_WAVEFROM))
		{
			pWaveForm->m_type = e.attribute(TAGNAME_TYPE);
			pWaveForm->m_units = e.attribute(TAGNAME_UNITS);
			QDomNode n = e.firstChild();
			while (!n.isNull())
			{
				if (IsEqual(n.toElement().tagName(), TAGNAME_FILTERS))
				{
					ParseFiltersNode(n, &pWaveForm->m_filters);
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_WAVEFORMSEGMENT))
				{
					STUWaveformSegment* pSeg = new STUWaveformSegment;
					ParseWaveformSegmentNode(n, pSeg);
					pWaveForm->m_waveformSegments.push_back(pSeg);
				}
				else
				{
					//新元素
					UnknownNodeFilter(n);
				}
				n = n.nextSibling();
			}
		}
		else
		{
			bRet = false;
			Q_ASSERT(false);
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseFiltersNode(const QDomNode& filterNode, STUFilters* pFilters)
{
	bool bRet = true;
	if (NULL != pFilters)
	{
		QDomElement e = filterNode.toElement();
		if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_FILTERS))
		{
			QDomNode n = e.firstChild();
			while(!n.isNull())
			{
				if (IsEqual(n.toElement().tagName(), TAGNAME_HIGHPASS))
				{
					pFilters->m_highPass = n.toElement().text();
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_LOWPASS))
				{
					pFilters->m_lowPass = n.toElement().text();
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_BASELINEDRIFTREMOVAL))
				{
					pFilters->m_baselineDriftRemoval = n.toElement().text();
				}
				else
				{
					//新元素
					UnknownNodeFilter(n);
				}
				n = n.nextSibling();
			}
		}
		else
		{
			bRet = false;
			Q_ASSERT(false);
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

//
bool QExportXmlParser::ParseWaveformSegmentNode(const QDomNode& waveformSegNode, STUWaveformSegment* pSeg)
{
	bool bRet = true;
	if (NULL != pSeg)
	{
		QDomElement e = waveformSegNode.toElement();
		if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_WAVEFORMSEGMENT))
		{
			pSeg->m_time = e.attribute(TAGNAME_TIME);
			QDomNode n = waveformSegNode.firstChild();
			while (!n.isNull())
			{
				if (IsEqual(n.toElement().tagName(), TAGNAME_SAMPLERATE))
				{
					pSeg->m_sampleRate = n.toElement().text();
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_DATARESOLUTION))
				{
					pSeg->m_dataResolation = n.toElement().text();
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_DATA))
				{
					pSeg->m_data = n.toElement().text();
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_ANNOTATION))
				{
					STUAnnotation* pAnnotation = new STUAnnotation;
					ParseAnnotationNode(n, pAnnotation);
					pSeg->m_annotation.push_back(pAnnotation);
				}
				else
				{
					//新元素
					UnknownNodeFilter(n);
				}
				n = n.nextSibling();
			}
		}
		else
		{
			bRet = false;
			Q_ASSERT(false);
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseAnnotationNode(const QDomNode& annotationNode, STUAnnotation* pAnnotation)
{
	bool bRet = true;
	if (NULL != pAnnotation)
	{
		QDomElement e = annotationNode.toElement();
		if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_ANNOTATION))
		{
			pAnnotation->m_type = e.attribute(TAGNAME_TYPE);
			QDomNode n = annotationNode.firstChild();
			while (!n.isNull())
			{
				if (IsEqual(n.toElement().tagName(), TAGNAME_SAMPLE))
				{
					pAnnotation->m_offset = n.toElement().attribute(TAGNAME_OFFSET);
				}
				else
				{
					//新元素
					UnknownNodeFilter(n);
				}
				n = n.nextSibling();
			}
		}
		else
		{
			bRet = false;
			Q_ASSERT(false);
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseObservationNode(const QDomNode& observationNode, STUObservation* pObservation)
{
	bool bRet = true;
	if (NULL != pObservation)
	{
		QDomElement e = observationNode.toElement();
		if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_OBSERVATION))
		{
			pObservation->m_type = e.attribute(TAGNAME_TYPE);
			pObservation->m_channel = e.attribute(TAGNAME_CHANNEL);
			pObservation->m_lead = e.attribute(TAGNAME_LEAD);
			pObservation->m_location = e.attribute(TAGNAME_LOCATION);
			pObservation->m_units = e.attribute(TAGNAME_UNITS);
			
			QDomNode n = observationNode.firstChild();
			if (!n.isNull())
			{
				pObservation->m_value = e.toElement().text();
			}
		}
		else if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_QTCFORMULA))
		{
			pObservation->m_type = TAGNAME_QTCFORMULA;
			QDomNode n = observationNode.firstChild();
			if (!n.isNull())
			{
				pObservation->m_value = e.toElement().text();
			}
		}
		else
		{
			bRet = false;
			Q_ASSERT(false);
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}


bool QExportXmlParser::ParsePatientNode(const QDomNode& patientNode, STUPatient* pPatient)
{
	bool bRet = true;
	if (NULL != pPatient)
	{
		QDomElement e = patientNode.toElement();
		if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_PATIENT))
		{
			QDomNode n = patientNode.firstChild();
			while (!n.isNull())
			{
				if (IsEqual(n.toElement().tagName(), TAGNAME_DEMOGRAPHICS))
				{
					ParseDemographicsNode(n, pPatient->m_demographics);
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_ASSIGNEDLOCATION))
				{
					ParseAssignedLocationNode(n, pPatient->m_assignedLocation);
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_DOCTOR))
				{
					ParseDoctorNode(n, pPatient->m_doctor);
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_TECHNICIAN))
				{
					ParseTechnicianNode(n, pPatient->m_technician);
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_PACED))
				{
					pPatient->m_paced = n.toElement().text();
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_MEDICATIONS))
				{
					ParseMedicationsNode(n, pPatient->m_medications);
				}
				else if (IsEqual(n.toElement().tagName(), TAGNAME_CLINICALCLASSIFICATIONS))
				{
					ParseClinicalClassificationsNode(n, pPatient->m_clinicalClassfications);
				}
				else
				{
					//新元素
					UnknownNodeFilter(n);
				}
				n = n.nextSibling();
			}
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseDemographicsNode(const QDomNode& demographicsNode, STUDemographics& demographics)
{
	bool bRet = true;
	QDomElement e = demographicsNode.toElement();
	if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_DEMOGRAPHICS))
	{
		QDomNode n = demographicsNode.firstChild();
		while (!n.isNull())
		{
			if (IsEqual(n.toElement().tagName(), TAGNAME_FIRSTNAME))
			{
				demographics.m_firstName = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_LASTNAME))
			{
				demographics.m_lastName = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_PATIENTID))
			{
				demographics.m_patientID = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_DATEOFBIRTH))
			{
				demographics.m_dateOfBirth = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_AGE))
			{
				demographics.m_age = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_GENDER))
			{
				demographics.m_gender = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_RACE))
			{
				demographics.m_race = n.toElement().text();
			}
			else
			{
				//新元素
				UnknownNodeFilter(n);
			}
			n = n.nextSibling();
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseAssignedLocationNode(const QDomNode& assignedLocationNode, STUAssignedLocation& assignedLocation)
{
	bool bRet = true;
	QDomElement e = assignedLocationNode.toElement();
	if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_ASSIGNEDLOCATION))
	{
		QDomNode n = assignedLocationNode.firstChild();
		while (!n.isNull())
		{
			if (IsEqual(n.toElement().tagName(), TAGNAME_BED))
			{
				assignedLocation.m_bed = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_ROOM))
			{
				assignedLocation.m_room = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_POINTOFCARE))
			{
				assignedLocation.m_pointOfCare = n.toElement().text();
			}
			else if (IsEqual(n.toElement().tagName(), TAGNAME_FACILITY))
			{
				assignedLocation.m_facility = n.toElement().text();
			}
			else
			{
				//新元素
				UnknownNodeFilter(n);
			}
			n = n.nextSibling();
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseDoctorNode(const QDomNode& doctorNode, STUDoctor& doctor)
{
	bool bRet = true;
	QDomElement e = doctorNode.toElement();
	if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_DOCTOR))
	{
		QDomNode n = doctorNode.firstChild();
		while (!n.isNull())
		{
			if (IsEqual(n.toElement().tagName(), TAGNAME_NAME))
			{
				doctor.m_name = n.toElement().text();
			}
			else
			{
				//新元素
				UnknownNodeFilter(n);
			}
			n = n.nextSibling();
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseTechnicianNode(const QDomNode& technicianNode, STUTechnician& technician)
{
	bool bRet = true;
	QDomElement e = technicianNode.toElement();
	if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_TECHNICIAN))
	{
		QDomNode n = technicianNode.firstChild();
		while (!n.isNull())
		{
			if (IsEqual(n.toElement().tagName(), TAGNAME_NAME))
			{
				technician.m_name = n.toElement().text();
			}
			else
			{
				//新元素
				UnknownNodeFilter(n);
			}
			n = n.nextSibling();
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseMedicationsNode(const QDomNode& medicationsNode, STUMedications& medications)
{
	bool bRet = true;
	QDomElement e = medicationsNode.toElement();
	if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_MEDICATIONS))
	{
		QDomNode n = medicationsNode.firstChild();
		while (!n.isNull())
		{
			if (IsEqual(n.toElement().tagName(), TAGNAME_MEDICATION))
			{
				medications.m_name.append(n.toElement().attribute(TAGNAME_NAME));
			}
			else
			{
				//新元素
				UnknownNodeFilter(n);
			}
			n = n.nextSibling();
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

bool QExportXmlParser::ParseClinicalClassificationsNode(const QDomNode& clinicalClassificationsNode, STUClinicalClassifications& clinicalClassifications)
{
	bool bRet = true;
	QDomElement e = clinicalClassificationsNode.toElement();
	if (!e.isNull() && IsEqual(e.tagName(), TAGNAME_CLINICALCLASSIFICATIONS))
	{
		QDomNode n = clinicalClassificationsNode.firstChild();
		while (!n.isNull())
		{
			if (IsEqual(n.toElement().tagName(), TAGNAME_CLINICALCLASSIFICATION))
			{
				clinicalClassifications.m_classifications.append(n.toElement().text());
			}
			else
			{
				//新元素
				UnknownNodeFilter(n);
			}
			n = n.nextSibling();
		}
	}
	else
	{
		bRet = false;
		Q_ASSERT(false);
	}
	return bRet;
}

void QExportXmlParser::UnknownNodeFilter(const QDomNode& n)
{	
	QString tagName = n.toElement().tagName();
	if (tagName.isEmpty())
	{
		//应该是XML注释节点。忽略
	}
	else
	{
		Q_ASSERT(false);
		m_unknownTags += tr("%1; ").arg(tagName);
	}
}

bool QExportXmlParser::IsEqual(const QString& l, const QString& r, Qt::CaseSensitivity cs)
{
	return 0==l.compare(r, cs);
}

const QVector<STUWaveform*>& QExportXmlParser::GetWaveforms() const
{
	return m_waveforms;
}

const QVector<STUWaveform*>& QExportXmlParser::GetMedianBeatTemplates() const
{
	return m_medianBeatTemp;
}

const QVector<STUObservation*>& QExportXmlParser::GetObservations() const
{
	return m_observations;
}

const STUPatient* QExportXmlParser::GetPatient() const
{
	return m_patientPtr;
}