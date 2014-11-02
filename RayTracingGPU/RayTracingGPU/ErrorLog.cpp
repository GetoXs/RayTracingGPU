#include "stdafx.h"
#include "ErrorLog.h"


const char *ErrorLog::LogDir = "Logs\\Error";
const char *ErrorLog::LogFileName = "ErrorLog.txt";


void ErrorLog::AppendError(char *type, char *entry)
{
	QDateTime currentDT = QDateTime::currentDateTime();
	QString configFileName = currentDT.toString("yyyyMMdd-HHmmss_") + ErrorLog::LogFileName;
	QString configFilePath = QString(ErrorLog::LogDir) + "\\" + configFileName;
	//tworzenie katalogu
	QDir d = QFileInfo(configFilePath).dir();
	bool res = QDir().mkpath(d.path());

	//otwarcie pliku
	QFile raportFile(configFilePath);
	if (raportFile.open(QFile::Append | QFile::Text))
	{
		QTextStream txt(&raportFile);
		txt.setCodec("UTF-8");
		txt << currentDT.toString(Qt::DateFormat::ISODate) << "\t" << type << "\t" << entry << endl;

		raportFile.close();
	}
}
