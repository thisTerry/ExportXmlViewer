#ifndef QPARAMETERLISTWIDGET_H
#define QPARAMETERLISTWIDGET_H
struct STUObservation;
#include <QTableWidget>

/*
	参数列表控件。
	专用于显示导出XML中的参数。
*/
class QParameterListWidget : public QTableWidget
{
	Q_OBJECT
public:
	QParameterListWidget(QWidget* parent);
	~QParameterListWidget();

	void FillObservations(const QVector<STUObservation*>& observations);

public:
	virtual QSize sizeHint() const;
	virtual void setVisible(bool visible);

private:
	void InitComponent();

private:
	Q_DISABLE_COPY(QParameterListWidget);
};

#endif //QPARAMETERLISTWIDGET_H