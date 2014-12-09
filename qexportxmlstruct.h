#ifndef QEXPORTXMLSTRUCT_H
#define QEXPORTXMLSTRUCT_H

#include <QVector>
#include <QStringList>

struct STUFilters
{
	QString m_highPass;
	QString m_lowPass;
	QString m_baselineDriftRemoval;
};

struct STUAnnotation
{
	QString m_type;
	QString m_offset;
};

struct STUWaveformSegment
{
	QString m_time;
	QString m_sampleRate;
	QString m_dataResolation;
	QString m_data;
	QVector<STUAnnotation*> m_annotation;

	STUWaveformSegment(){};
	~STUWaveformSegment()
	{
		for (QVector<STUAnnotation*>::iterator it=m_annotation.begin(); it!=m_annotation.end(); it++)
		{
			delete *it;
			*it = NULL;
		}
		m_annotation.clear();
	}
};

struct STUWaveform
{
	QString m_type;
	QString m_units;
	STUFilters m_filters;
	QVector<STUWaveformSegment*> m_waveformSegments;

	STUWaveform(){};
	~STUWaveform()
	{
		for (QVector<STUWaveformSegment*>::iterator it=m_waveformSegments.begin(); it!=m_waveformSegments.end(); it++)
		{
			delete *it;
			*it = NULL;
		}
		m_waveformSegments.clear();
	}
};

//////////////////////////////////////////////////////////////////////////
struct STUObservation
{
	QString m_type;
	QString m_channel;
	QString m_lead;
	QString m_location;
	QString m_units;
	QString m_value;

	STUObservation(){};
	~STUObservation(){};
};

//////////////////////////////////////////////////////////////////////////
struct STUDemographics
{
	QString m_firstName;
	QString m_lastName;
	QString m_patientID;
	QString m_dateOfBirth;
	QString m_age;
	QString m_gender;
	QString m_race;
};

struct STUAssignedLocation
{
	QString m_bed;
	QString m_room;
	QString m_pointOfCare;
	QString m_facility;
};

struct STUDoctor
{
	QString m_name;
};
struct STUTechnician
{
	QString m_name;
};
struct STUMedications
{
	QStringList m_name;
};

struct STUClinicalClassifications
{
	QStringList m_classifications;
};

struct STUPatient
{
	struct STUDemographics m_demographics;
	struct STUAssignedLocation m_assignedLocation;
	struct STUDoctor m_doctor;
	struct STUTechnician m_technician;
	QString m_paced;
	struct STUMedications m_medications;
	struct STUClinicalClassifications m_clinicalClassfications;
};

#endif // QEXPORTXMLSTRUCT_H
