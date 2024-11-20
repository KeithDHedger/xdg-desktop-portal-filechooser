/*
 *
 * ©K. D. Hedger. Wed 20 Nov 13:41:52 GMT 2024 keithdhedger@gmail.com

 * This file (qtdialog.cpp) is part of xdg-desktop-portal-filechooser.

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

#include <QtGui>
#include <QApplication>
#include <QLabel>
#include <QFileDialog>

enum {PATH=1,MULTIPLE,DIRECTORY,SAVE,FILENAME,FILTER};

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("KDHedger");
	app.setApplicationName("TestPrefsClass");
	QIcon::setFallbackThemeName("gnome");

	if((QString(argv[SAVE]).isEmpty()==true) && (QString(argv[DIRECTORY]).isEmpty()==true))
		{
		//run load;
			QString filters="";
			if(QString(argv[DIRECTORY]).isEmpty()==true)
				{
					for(int j=FILTER;j<argc;j++)
						{
							QString	filt(argv[j]);
							if(filt.isEmpty()==false)
								{
									if(filt.contains("All files",Qt::CaseInsensitive)==false)
										filters+=(filt)+";;";
								}
						}
					filters+=("All Files (*)");
				}
			if(QString(argv[MULTIPLE]).isEmpty()==true)
				{
					QString filename=QFileDialog::getOpenFileName(nullptr,"Open File",QString(argv[PATH]),filters,nullptr,QFileDialog::DontUseNativeDialog);
					if(filename.isEmpty()==false)
						printf("%s\n",filename.toStdString().c_str());
				}
			else
				{
					QStringList files=QFileDialog::getOpenFileNames(nullptr,"Open Files",QString(argv[PATH]),filters,nullptr,QFileDialog::DontUseNativeDialog);
					if(files.isEmpty()==false)
						{
							for(int j=0;j<files.count();j++)
								printf("%s\n",files.at(j).toStdString().c_str());
							printf("\n");
						}
				}
		}


	else if(QString(argv[SAVE]).isEmpty()==false)
		{
		//run save
			QString filters="";
			for(int j=FILTER;j<argc;j++)
				{
					QString	filt(argv[j]);
					if(filt.isEmpty()==false)
						{
							if(filt.contains("All files",Qt::CaseInsensitive)==false)
								filters+=(filt)+";;";
						}
				}
			filters+=("All Files (*)");
			QString filename=QFileDialog::getSaveFileName(nullptr,"Save File",QString(argv[PATH]),filters,nullptr,QFileDialog::DontUseNativeDialog);
			if(filename.isEmpty()==false)
				printf("%s\n",filename.toStdString().c_str());
		}
	else
		{
		//run folder
			QString filename=QFileDialog::getExistingDirectory(nullptr,"Open Folder",QString(argv[PATH]),QFileDialog::DontUseNativeDialog|QFileDialog::ShowDirsOnly);
			if(filename.isEmpty()==false)
				printf("%s\n",filename.toStdString().c_str());
		}
	return(0);
}