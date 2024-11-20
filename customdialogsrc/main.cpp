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

#include <QApplication>
#include <QSettings>

#include "ChooserDialog.h"

enum {PATH=1,MULTIPLE,DIRECTORY,SAVE,FILENAME,FILTER};

int main(int argc, char **argv)
{
	QApplication			app(argc,argv);
	chooserDialogType	dt;

	if(QString(argv[SAVE]).isEmpty()==true)
		dt=chooserDialogType::loadDialog;
	else
		dt=chooserDialogType::saveDialog;

	if(QString(argv[DIRECTORY]).isEmpty()==false)
		dt=chooserDialogType::folderDialog;

	chooserDialogClass	chooser(dt,QString(argv[FILENAME]),QString(argv[PATH]));

	if(QString(argv[FILENAME]).isEmpty()==false)
		chooser.fileNameEdit.setText(QString(argv[FILENAME]));

	if(QString(argv[DIRECTORY]).isEmpty()==true)
		chooser.gFind.LFSTK_setFindType(FILETYPE);
	else
		{
			chooser.gFind.LFSTK_setFindType(FOLDERTYPE);
			chooser.getFolder=true;
		}

	chooser.gFind.LFSTK_setFullPath(true);
	chooser.gFind.LFSTK_setIgnoreNavLinks(false);
	chooser.gFind.LFSTK_sortByName();
	chooser.setShowImagesInList(true);

	if(QString(argv[MULTIPLE]).isEmpty()==false)
		chooser.setMultipleSelect(true);

	if(QString(argv[DIRECTORY]).isEmpty()==true)
		{
			for(int j=FILTER;j<argc;j++)
				{
					QString	filt(argv[j]);
					if(filt.isEmpty()==false)
						{
							if(filt.contains("All files",Qt::CaseInsensitive)==false)
								chooser.addFileTypes(filt);
						}
				}
			chooser.addFileTypes("All Files");
		}
	else
		chooser.fileTypes.hide();

	chooser.dialogWindow.exec();

	qDebug()<<"Dir:"<<chooser.startDir;
	qDebug()<<"Filename:"<<chooser.selectedFileName;
	qDebug()<<"Filepath:"<<chooser.selectedFilePath;
	qDebug()<<"Cannonical dir:"<<chooser.realFolderPath;
	qDebug()<<"Cannonical name:"<<chooser.realName;
	qDebug()<<"Cannonical filepath:"<<chooser.realFilePath;
	qDebug()<<"File exists:"<<chooser.fileExists;
	qDebug()<<"Valid:"<<chooser.valid;

	for(int j=0;j<chooser.multiFileList.count();j++)
		qDebug()<<"Multi:"<<chooser.multiFileList.at(j);

//if(chooser.getFolder==false)
	if(chooser.valid==true)
		{
			if(chooser.useMulti==true)
				{
					for(int j=0;j<chooser.multiFileList.count();j++)
						//printf("%s:/:",chooser.multiFileList.at(j).toStdString().c_str());
						//printf("%s\0\n\0",chooser.multiFileList.at(j).toStdString().c_str());
						//printf("%s\0\0\0\n",chooser.multiFileList.at(j).toStdString().c_str());
						printf("%s\n",chooser.multiFileList.at(j).toStdString().c_str());
					//printf(":/:");
					//printf("\0");
					printf("\n");
				}
			else
				printf("%s",chooser.selectedFilePath.toStdString().c_str());
		}
//else
//	printf("%s",chooser.realFolderPath.toStdString().c_str());

	return (0);
}