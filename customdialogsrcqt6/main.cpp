/*
 *
 * ©K. D. Hedger. Wed 20 Nov 14:37:17 GMT 2024 keithdhedger@gmail.com

 * This file (main.cpp) is part of xdg-desktop-portal-filechooser.

 * xdg-desktop-portal-filechooser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * xdg-desktop-portal-filechooser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with xdg-desktop-portal-filechooser.  If not, see <http://www.gnu.org/licenses/>.
*/

//#include <QApplication>
//#include <QSettings>

#include "globals.h"

enum {PATH=1,MULTIPLE,DIRECTORY,SAVE,FILENAME,FILTER};

int main(int argc, char **argv)
{
	QApplication			app(argc,argv);

	chooserDialogType	type;
	bool					addallfiles=true;

	if(QString(argv[SAVE]).isEmpty()==true)
		type=chooserDialogType::loadDialog;
	else
		type=chooserDialogType::saveDialog;

	if(QString(argv[DIRECTORY]).isEmpty()==false)
		type=chooserDialogType::folderDialog;

	chooserDialogClass	chooser(type,QString(argv[FILENAME]),QString(argv[PATH]));

	if(QString(argv[MULTIPLE]).isEmpty()==false)
		chooser.setMultipleSelect(true);

	if(QString(argv[DIRECTORY]).isEmpty()==true)
		{
			for(int j=FILTER;j<argc;j++)
				{
					QString	filt(argv[j]);
					if(filt.isEmpty()==false)
						chooser.addFileTypes(filt);
					if(filt.simplified().startsWith("All Files",Qt::CaseInsensitive))
						addallfiles=false;
				}
			if(addallfiles==true)
				chooser.addFileTypes("All Files");
		}
	chooser.setShowImagesInList(true);

	chooser.dialogWindow.exec();
//TODO//
////wait for image loader thread to quit
//	if(chooser.running==true)
//		{
//			chooser.running=false;
//			chooser.isdone=false;
//			while(chooser.isdone==false);
//		}
//	qDebug()<<"Dir:"<<chooser.startDir;
//	qDebug()<<"Filename:"<<chooser.selectedFileName;
//	qDebug()<<"Filepath:"<<chooser.selectedFilePath;
//	qDebug()<<"Cannonical dir:"<<chooser.realFolderPath;
//	qDebug()<<"Cannonical name:"<<chooser.realName;
//	qDebug()<<"Cannonical filepath:"<<chooser.realFilePath;
//	qDebug()<<"File exists:"<<chooser.fileExists;
//	qDebug()<<"Valid:"<<chooser.valid;
//
//	for(int j=0;j<chooser.multiFileList.count();j++)
//		qDebug()<<"Multi:"<<chooser.multiFileList.at(j);


	for(const QString &str : chooser.multiFileList)
		printf("%s\n",qPrintable(str));
	printf("\n");

	return (0);
}