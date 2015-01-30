#include "QExportXmlViewer.h"
#include <QtWidgets/QApplication>
#include "CFGMgr.h"

static const QString ORG_NAME      =  "Mindray";
static const QString APP_NAME      =  "ExportXmlViewer";

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName(ORG_NAME);
	QCoreApplication::setApplicationName(APP_NAME);

	CFGMgr::Instance();

	QExportXmlViewer w;
	w.resize(1024, 750);
	w.show();

	CFGMgr::ReleaseInstance();
	return a.exec();
}
