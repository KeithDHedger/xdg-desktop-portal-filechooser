/*
 *
 * ©K. D. Hedger. Wed 20 Nov 14:35:50 GMT 2024 keithdhedger@gmail.com

 * This file (ChooserDialog.cpp) is part of xdg-desktop-portal-filechooser.

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

#include "ChooserDialog.h"

chooserDialogClass::~chooserDialogClass()
{
}

void chooserDialogClass::setShowImagesInList(bool show)
{
	this->showThumbsInList=show;
	this->setFileList();
}

void chooserDialogClass::setMultipleSelect(bool select)
{
	if(this->dialogType==chooserDialogType::saveDialog)
		return;

	this->useMulti=select;
	if(select==true)
		this->fileList.setSelectionMode(QAbstractItemView::ExtendedSelection);
	else
		this->fileList.setSelectionMode(QAbstractItemView::SingleSelection);
}

QIcon chooserDialogClass::getFileIcon(QString path)
{
	QIcon				icon;
	QMimeDatabase		db;
	QString				realpath(QFileInfo(path).canonicalFilePath());
	QMimeType			type;

	if(realpath.isEmpty()==true)
		type=db.mimeTypeForFile(path);
	else
		type=db.mimeTypeForFile(realpath);

	if(type.name().compare("application/x-desktop")==0)
		{
			QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << "/usr/share/pixmaps");
			std::map<unsigned long,std::vector<std::string>> entrys=LFSTK_UtilityClass::LFSTK_readFullDesktopFile(path.toStdString());
			std::string e;
			e=LFSTK_UtilityClass::LFSTK_getFullEntry("Desktop Entry","Icon",entrys,true);
			icon=QIcon::fromTheme(e.c_str());
		}
	else
		{
			if(type.name().contains("image"))
				{
					icon=QIcon::fromTheme("image");
				}
			else
				{
					icon=QIcon::fromTheme(type.iconName());
				}
		}
	if(icon.isNull()==true)
		icon=QIcon::fromTheme("application-octet-stream");

	return(icon);
}

void chooserDialogClass::setSideListMode(QListView::ViewMode mode)
{
	this->sideList.setViewMode(mode);
	this->sideList.setGridSize(QSize(96,48));
	this->sideList.setResizeMode(QListView::Adjust);
}

void chooserDialogClass::setFileListMode(QListView::ViewMode mode)
{
	this->fileList.setViewMode(mode);
}

void chooserDialogClass::updateImagesThread(void)
{
	QString			t;
	QMimeDatabase	db;
	QStandardItem	*si=NULL;
	QString			realpath;
	QMimeType		type;
	QPixmap			pms;

	this->running=true;
	this->isdone=false;
	for(int j=0;j<this->fileListModel->rowCount();j++)
		{
			if(this->running==false)
				{
					this->isdone=true;
					this->running=false;
					return;
				}
			si=this->fileListModel->item(j);
			if(si!=nullptr)
				{
					t=QFileInfo(this->startDir+"/"+si->data(Qt::UserRole).toString()).absoluteFilePath();
					realpath=QFileInfo(t).canonicalFilePath();

					if(realpath.isEmpty()==true)
						continue;

					type=db.mimeTypeForFile(realpath);

					if(type.name().contains("image"))
						{
							pms=QPixmap(realpath).scaled(128,128,Qt::IgnoreAspectRatio,Qt::FastTransformation) ;
							QIcon ic(pms);
							if(ic.isNull()==false)
								{
									if(this->running!=false)
										si->setIcon(ic);
								}
						}
				}
		}
	this->isdone=true;
	this->running=false;
}

void chooserDialogClass::setFileList(void)
{
	QStandardItem	*item;
	QFuture<void>	future;

//wait for image loader thread to quit
	if(this->running==true)
		{
			this->running=false;
			this->isdone=false;
			while(this->isdone==false);
		}

	this->fileListModel->clear();
	this->gFind.deleteData();
	this->gFind.LFSTK_setIncludeHidden(this->showHidden);
	this->gFind.LFSTK_findFiles(this->startDir.toStdString().c_str());
	this->gFind.LFSTK_sortByTypeAndName();

	for(int j=0;j<gFind.LFSTK_getDataCount();j++)
		{
			if((gFind.data.at(j).fileType==FILELINKTYPE) || (gFind.data.at(j).fileType==FOLDERLINKTYPE))
				{
					item=new QStandardItem(this->getFileIcon(this->startDir+"/"+gFind.data.at(j).name.c_str()),QString("->%1").arg(gFind.data.at(j).name.c_str()));
					item->setFont(QFont(item->font().family(),-1,QFont::Bold));
				}
			else if(gFind.data.at(j).fileType==BROKENLINKTYPE)
				{
					item=new QStandardItem(this->getFileIcon(this->startDir+"/"+gFind.data.at(j).name.c_str()),QString("%1 - Broken Link").arg(gFind.data.at(j).name.c_str()));
					item->setFont(QFont(item->font().family(),-1,QFont::Bold));
				}
			else
				{
					item=new QStandardItem(this->getFileIcon(this->startDir+"/"+gFind.data.at(j).name.c_str()),gFind.data.at(j).name.c_str());
				}
			if((gFind.data.at(j).fileType==FOLDERTYPE) || (gFind.data.at(j).fileType==FOLDERLINKTYPE))
				item->setDragEnabled(true);
			else
				item->setDragEnabled(false);
			
			item->setData(gFind.data.at(j).name.c_str(),Qt::UserRole);
			item->setStatusTip(this->startDir+"/"+gFind.data.at(j).name.c_str());
			this->fileListModel->appendRow(item);
		}
	this->fileList.scrollToTop();
	this->setFolderPathsDrop();
	future=QtConcurrent::run(this,&chooserDialogClass::updateImagesThread);
}

void chooserDialogClass::setSideList(void)
{
	QVariant				fullFilePathData;
	QStorageInfo			storage;
	QStandardItem		*item;
	QList<QStorageInfo>	ml=QStorageInfo::mountedVolumes();
	QString				disktype;
	QSettings			prefs("KDHedger","ChooserDialog");//TODO//
	QStringList			sl=prefs.value("customfolders").toStringList();

///standard items
	this->sideListModel->clear();
	item=new QStandardItem(QIcon::fromTheme("computer"),"Computer");
	fullFilePathData="/";
	item->setData(fullFilePathData,Qt::UserRole);
	this->sideListModel->appendRow(item);
	fullFilePathData=QDir::homePath();
	item=new QStandardItem(QIcon::fromTheme("user-home"),QFileInfo(QDir().homePath()).baseName());
	item->setData(fullFilePathData,Qt::UserRole);
	this->sideListModel->appendRow(item);
	
//recent folders
	item=new QStandardItem(QIcon::fromTheme("folder-saved-search"),"Recent Folders");
	fullFilePathData=this->recentFoldersPath;
	item->setData(fullFilePathData,Qt::UserRole);
	this->sideListModel->appendRow(item);

//recent files
	item=new QStandardItem(QIcon::fromTheme("folder-saved-search"),"Recent Files");
	fullFilePathData=this->recentFilesPath;
	item->setData(fullFilePathData,Qt::UserRole);
	this->sideListModel->appendRow(item);

	item=new QStandardItem("");
	item->setEnabled(false);
	this->sideListModel->appendRow(item);

//mounted drives	 
	for(int j=0;j<ml.size();j++)
		{
			storage=ml.at(j);
			if((storage.fileSystemType().compare("tmpfs")!=0) && (storage.rootPath().compare("/")!=0) && (storage.rootPath().compare(QDir().homePath())!=0))
				{
					disktype="drive-harddisk";
					if(storage.fileSystemType().contains("nfs"))
						disktype="folder-remote";
					if(storage.fileSystemType().contains("ssh"))
						disktype="network_local";
					fullFilePathData=storage.rootPath();
					item=new QStandardItem(QIcon::fromTheme(disktype),QFileInfo(storage.rootPath()).baseName());
					item->setData(fullFilePathData,Qt::UserRole);
					this->sideListModel->appendRow(item);
				}
		}
	item=new QStandardItem("");
	item->setEnabled(false);
	this->sideListModel->appendRow(item);

//favs
	for(int j=0;j<sl.size();j++)
		{
			item=new QStandardItem(QIcon::fromTheme("user-bookmarks"),QFileInfo(sl.at(j)).fileName());
			item->setData(QFileInfo(sl.at(j)).fileName(),Qt::UserRole);
			item->setStatusTip(sl.at(j));
			this->sideListModel->appendRow(item);	
		}
}

void chooserDialogClass::showPreViewData(void)
{
	if(this->selectedFilePath.isEmpty()==true)
		return;

	QIcon			icon;
	QPixmap			pixmap;
	QMimeDatabase	db;
	QString			mod;
	int				md;
	struct stat		sb;
	QMimeType		type=db.mimeTypeForFile(this->selectedFilePath);

	this->previewMimeType.setText(type.name());

	if(type.name().contains("image"))
		{
			pixmap=QIcon(this->selectedFilePath).pixmap(QSize(128,128));
		}
	else
		{
			icon=this->getFileIcon(this->selectedFilePath);
			pixmap=icon.pixmap(QSize(128,128));
		}

	this->previewIcon.setPixmap(pixmap);
	this->previewSize.setText(QString("Size: %1").arg(QFileInfo(this->selectedFilePath).size()));

	if(lstat(this->selectedFilePath.toStdString().c_str(),&sb)!=-1)
		{
			md=sb.st_mode & 07777;
			mod.setNum(md,8);
			this->previewMode.setText(QString("Mode: %1").arg(mod));
		}
}

void chooserDialogClass::selectItem(const QModelIndex &index)
{
	QString	t;
	t=QFileInfo(this->startDir+"/"+index.data(Qt::UserRole).toString()).absoluteFilePath();

	if((QFileInfo(t).isDir()==false) && (this->dialogType!=chooserDialogType::folderDialog))//TODO//NULL test?
		this->fileNameEdit.setText(index.data(Qt::UserRole).toString());
	else
		{
			if(this->dialogType==chooserDialogType::folderDialog)
				{
					if(index.data(Qt::UserRole).toString().compare("..")!=0)
						this->fileNameEdit.setText(index.data(Qt::UserRole).toString());
				}
		}

	this->selectedFileName=index.data(Qt::UserRole).toString();
	this->selectedFilePath=t;
	this->realFilePath=QFileInfo(this->selectedFilePath).canonicalFilePath();
	this->realFolderPath=QFileInfo(this->realFilePath).canonicalPath();
	this->realName=QFileInfo(this->realFilePath).fileName();;

	this->showPreViewData();
}

void chooserDialogClass::selectSideItem(const QModelIndex &index)
{
	QList<QStorageInfo>		ml=QStorageInfo::mountedVolumes();
	QStorageInfo				storage;
	QString					disktype;
	QPixmap					pixmap;
	QIcon					icon; 
	QLocale					locale;
	QString					type;
	qint64					sze=0;
	qint64					freeb=0;
	const QAbstractItemModel	*model;
	QMap<int,QVariant>		map;
	int						itemoffset=2;

	disktype="drive-harddisk";

	switch(index.row())
		{
			case 0:
				disktype="computer";
				storage=ml.at(index.row());
				sze=storage.bytesTotal();
				type=storage.fileSystemType();
				freeb=storage.bytesFree();
				break;
			case 1:
				disktype="user-home";
				type="";
				sze=0;
				freeb=0;
				break;
			case 2:
				disktype="folder-saved-search";
				type="";
				sze=0;
				freeb=0;
				break;
			case 3:
				disktype="folder-saved-search";
				type="";
				sze=0;
				freeb=0;
				break;
			case 4:
				return;
			default:
				if((index.row()-itemoffset)<ml.size())
					{
						storage=ml.at(index.row()-itemoffset);
						if((storage.rootPath().compare("/")!=0) && (storage.rootPath().compare(QDir().homePath())!=0))
							{
								if(storage.fileSystemType().contains("nfs"))
									disktype="folder-remote";
								if(storage.fileSystemType().contains("ssh"))
									disktype="network_local";	
								sze=storage.bytesTotal();
								type=storage.fileSystemType();
								freeb=storage.bytesFree();
							}
					}
				else
					{
						struct stat	sb;
						int			md;
						QString		mod;
						QString		str;

						model=index.model();
						map=model->itemData(index);
						if(map.find(Qt::StatusTipRole)!=map.end())
							{
								str=map[Qt::StatusTipRole].toString();
								if(str.isEmpty()==true)
									return;
								icon=QIcon::fromTheme("folder");
								pixmap=icon.pixmap(QSize(128,128));
								this->previewIcon.setPixmap(pixmap);
								this->previewSize.setText(QString("Size: %1").arg(QFileInfo(str).size()));
								this->previewMimeType.setText("inode/directory");
								if(lstat(str.toStdString().c_str(),&sb)!=-1)
									{
										md=sb.st_mode & 07777;
										mod.setNum(md,8);
										this->previewMode.setText(QString("Mode: %1").arg(mod));
									}
							}
						return;
					}
		}
	icon=QIcon::fromTheme(disktype);
	pixmap=icon.pixmap(QSize(128,128));
	this->previewIcon.setPixmap(pixmap);
	this->previewSize.setText(QString("Size: %1").arg(locale.formattedDataSize(sze)));
	this->previewMimeType.setText(QString("FS Type: %1").arg(type));
	this->previewMode.setText(QString("Free: %1").arg(locale.formattedDataSize(freeb)));
}

void chooserDialogClass::setFavs(void)
{
	QSettings			prefs("KDHedger","ChooserDialog");//TODO//
	QStringList			sl;
	QItemSelectionModel	*model;
	QModelIndexList		list;
	QString				filepath;

	this->sideList.setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->sideList.selectAll();
	model=this->sideList.selectionModel();
	list=model->selectedIndexes();

	for(int j=0;j<list.count();j++)
		{
			filepath=QFileInfo(list.at(j).data(Qt::StatusTipRole).toString()).absoluteFilePath();
			if(filepath.isEmpty()==false)
				sl<<filepath;
		}
	prefs.setValue("customfolders",sl);
}

void chooserDialogClass::setFileData(void)
{
	QString				fp;
	QItemSelectionModel	*model;
	QModelIndexList		list;
	QString				filepath;
	QSettings			prefs("KDHedger","ChooserDialog");
	QDir					tdir;
	QString				recentfiles;

	this->startDir=QFileInfo(this->startDir).absoluteFilePath();
	this->selectedFilePath=QFileInfo(this->startDir+"/"+this->fileNameEdit.text()).absoluteFilePath();
	this->selectedFileName=this->fileNameEdit.text();

	this->realName=QFileInfo(QFileInfo(this->selectedFilePath).canonicalFilePath()).fileName();
	this->realFilePath=QFileInfo(this->selectedFilePath).canonicalFilePath();

	fp=QFileInfo(this->selectedFilePath).canonicalFilePath();
	if(fp.isEmpty()==false)
		this->realFolderPath=QFileInfo(this->selectedFilePath).canonicalPath();
	else
		this->realFolderPath=QFileInfo(this->startDir).canonicalFilePath();

	this->fileExists=QFileInfo(this->realFilePath).exists();
	
	model=this->fileList.selectionModel();
	list=model->selectedIndexes();
	this->multiFileList.clear();

//TODO//
	if(list.count()>0)
		{
			for(int j=0;j<list.count();j++)
				{
					filepath=QFileInfo(this->startDir+"/"+list.at(j).data(Qt::UserRole).toString()).absoluteFilePath();
					if(QFileInfo(filepath).isDir()==false)
						{
							if(this->recentFilesPath.compare(this->startDir)==0)
								{
									this->multiFileList.push_back(QFileInfo(QString("%1").arg(filepath)).canonicalFilePath());
								}
							else
								{
									this->multiFileList.push_back(filepath);
								}
							QFile f(filepath);
							recentfiles=QString("%1/%2").arg(this->recentFilesPath).arg(list.at(j).data(Qt::UserRole).toString());
							f.link(recentfiles);
							f.setFileName(this->realFolderPath);
							recentfiles=QString("%1/%2").arg(this->recentFoldersPath).arg(QFileInfo(this->realFolderPath).fileName());
							f.link(recentfiles);
						}
				}
		}
	else
		{
			filepath=this->realFilePath;
			if(QFileInfo(filepath).isDir()==false)
				{
					if(this->recentFilesPath.compare(this->startDir)==0)
						{
							this->multiFileList.push_back(QFileInfo(QString("%1").arg(filepath)).canonicalFilePath());
						}
					else
						{
							this->multiFileList.push_back(filepath);
						}
					QFile f(filepath);
					recentfiles=QString("%1/%2").arg(this->recentFilesPath).arg(QFileInfo(filepath).fileName());
					f.link(recentfiles);
					f.setFileName(this->realFolderPath);
					recentfiles=QString("%1/%2").arg(this->recentFoldersPath).arg(QFileInfo(this->realFolderPath).fileName());
					f.link(recentfiles);
				}
		}
//	prefs.setValue("size",this->dialogWindow.size());
	QRect rg;
	QRect rf;
	rg=this->dialogWindow.geometry();
	rf=this->dialogWindow.frameGeometry();
	rf.setHeight(rf.height()-(rf.height()-rg.height()));
	rf.setWidth(rf.width()-(rf.width()-rg.width()));
	prefs.setValue("size",rf);

	switch(this->dialogType)
		{
			case chooserDialogType::loadDialog:
				prefs.setValue("lastloadfolder",this->startDir);
				break;				
			case chooserDialogType::saveDialog:
				prefs.setValue("lastsavefolder",this->startDir);
				break;
			case chooserDialogType::folderDialog:
				break;
		}
	this->setFavs();
}

void chooserDialogClass::setOverwriteWarning(bool warn)
{
	this->overwriteWarning=warn;
}

void chooserDialogClass::addFileTypes(QString types)
{
	this->fileTypes.addItem(types);
}

void chooserDialogClass::setFolderPathsDrop(void)
{
	QString	datapath=this->startDir;
	this->folderPaths.clear();
	this->folderPaths.addItem(datapath);
	while(datapath.length()>1)
		{
			datapath=QFileInfo(datapath).absolutePath();
			this->folderPaths.addItem(datapath);
		}
}

void chooserDialogClass::buildMainGui(void)
{
	QVBoxLayout	*windowvlayout=new QVBoxLayout;
	QVBoxLayout	*filevlayout=new QVBoxLayout;
	QVBoxLayout	*sidevlayout=new QVBoxLayout;
	QVBoxLayout	*infovlayout=new QVBoxLayout;
	QVBoxLayout	*controlsvlayout=new QVBoxLayout;
	QHBoxLayout	*hlayout=new QHBoxLayout;

	switch(this->dialogType)
		{
			case chooserDialogType::loadDialog:
				this->dialogWindow.setWindowTitle("Open File");
				break;
			case chooserDialogType::saveDialog:
				this->dialogWindow.setWindowTitle("Save File");
				break;
			case chooserDialogType::folderDialog:
				this->dialogWindow.setWindowTitle("Select Folder");
				break;
		}

	this->fileListModel=new QStandardItemModel(0,1);
    this->fileList.setModel(this->fileListModel);

//file list
	//this->fileList.setStyleSheet(QString("QFrame {border-width: 1px;border-color: palette(dark); border-style: solid;}"));
	QObject::connect(&this->fileList,&QListView::doubleClicked,[this](const QModelIndex &index)
		{
			QString	tdir;
			tdir=QFileInfo(this->startDir+"/"+index.data(Qt::UserRole).toString()).absoluteFilePath();

			if(QFileInfo(tdir).isDir()==false)
				{
					this->fileNameEdit.setText(index.data(Qt::UserRole).toString());
					this->setFileData();
					this->dialogWindow.hide();
				}
			else
				{
					this->startDir=tdir;
					if(this->dialogType==chooserDialogType::folderDialog)
						this->fileNameEdit.setText("");
					this->setFileList();
				}
		});

	QObject::connect(&this->fileList,&QListView::clicked,[this](const QModelIndex &index)
		{
			this->selectItem(index);
		});

//side list
	QObject::connect(&this->sideList,&QListView::clicked,[this](const QModelIndex &index)
		{
			this->selectSideItem(index);
		});

	QObject::connect(&this->sideList,&QListView::doubleClicked,[this](const QModelIndex &index)
		{
			const QAbstractItemModel	*model;
			model=index.model();
			QMap map(model->itemData(index));
			if(map.find(Qt::StatusTipRole)!=map.end())
				{
					QString str=map[Qt::StatusTipRole].toString();
					this->startDir=QFileInfo(str).absoluteFilePath();
				}
			else
				this->startDir=index.data(Qt::UserRole).toString();
			this->setFileList();
		});

	this->sideListModel=new QStandardItemModel(0,1);
    this->sideList.setModel(this->sideListModel);
	this->sideList.setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->fileList.setEditTriggers(QAbstractItemView::NoEditTriggers);

	sidevlayout->addWidget(&this->sideList);

	QPushButton *deletefav=new QPushButton("Remove Fav");
	deletefav->setIcon(QIcon::fromTheme("stock_cancel"));
	QObject::connect(deletefav,&QPushButton::clicked,[this]()
		{
			QModelIndex ind=this->sideList.currentIndex();
			if(ind.data(Qt::StatusTipRole).toString().isEmpty()==false)
				this->sideListModel->removeRow(this->sideList.currentIndex().row());
		});
	sidevlayout->addWidget(deletefav);

	filevlayout->addWidget(&this->fileList);

//preview
	this->previewIcon.setMaximumWidth(128);
	this->previewIcon.setMinimumWidth(128);
	this->previewIcon.setAlignment(Qt::AlignCenter);

	hlayout->addLayout(sidevlayout,1);
	hlayout->addLayout(filevlayout,3);
	hlayout->addLayout(infovlayout);
	
	this->previewMimeType.setWordWrap(true);
	infovlayout->addWidget(&this->previewIcon);
	infovlayout->addWidget(&this->previewMimeType);
	infovlayout->addWidget(&this->previewSize);
	infovlayout->addWidget(&this->previewMode);
	infovlayout->addStretch();

	windowvlayout->addLayout(hlayout);

//file paths
	controlsvlayout->addWidget(&this->folderPaths);
	QObject::connect(&this->folderPaths, QOverload<int>::of(&QComboBox::activated),[this](int index)
		{
			this->startDir=this->folderPaths.currentText();
			this->setFolderPathsDrop();
			this->setFileList();
		});

//file
	controlsvlayout->addWidget(&this->fileNameEdit);
	QObject::connect(&this->fileNameEdit,&QLineEdit::textChanged,[this](const QString &text)
		{
			this->selectedFilePath=text;
		});

	controlsvlayout->addWidget(&this->fileTypes);
	QObject::connect(&this->fileTypes,&QComboBox::currentTextChanged,[this](const QString &text)
		{
			if(text.compare("All Files")==0)
				this->gFind.LFSTK_setFileTypes("");
			else
				this->gFind.LFSTK_setFileTypes(text.toStdString());
			this->setFileList();
		});

	hlayout=new QHBoxLayout;
	QPushButton *cancel=new QPushButton("Cancel");
	cancel->setIcon(QIcon::fromTheme("stock_cancel"));
	QObject::connect(cancel,&QPushButton::clicked,[this]()
		{
			this->dialogWindow.hide();
			this->setFavs();
			this->valid=false;
		});

	QPushButton *hidden=new QPushButton("Hidden");
	hidden->setCheckable(true);
	hidden->setIcon(QIcon::fromTheme("stock_dialog_question"));
	QObject::connect(hidden,&QPushButton::clicked,[this]()
		{
			this->showHidden=!this->showHidden;
			this->setFileList();
		});

	QPushButton *newfolder=new QPushButton("New Folder");
	newfolder->setIcon(QIcon::fromTheme("folder-open"));
	QObject::connect(newfolder,&QPushButton::clicked,[this]()
		{
			bool		ok;
     		QString	text=QInputDialog::getText(&this->dialogWindow,"New Folder","New folder name",QLineEdit::Normal,"New Folder",&ok);
			if(ok==true)
				{
					QDir dirp(this->startDir);
					dirp.mkdir(text);
					this->startDir=QString("%1/%2").arg(this->startDir).arg(text);
					this->setFileList();
				}
		});

	QPushButton *refresh=new QPushButton("Refresh");
	refresh->setIcon(QIcon::fromTheme("refresh"));
	QObject::connect(refresh,&QPushButton::clicked,[this]()
		{
			this->setFileList();
		});

	QPushButton *apply=NULL;

	switch(this->dialogType)
		{
			case chooserDialogType::saveDialog:
				apply=new QPushButton("Save");
				break;
			case chooserDialogType::loadDialog:
				apply=new QPushButton("Open");
				break;
			case chooserDialogType::folderDialog:
				apply=new QPushButton("Select");
				break;
		}

	apply->setIcon(QIcon::fromTheme("stock_apply"));
	apply->setDefault(true);

	QObject::connect(apply,&QPushButton::clicked,[this]()
		{
			{
				switch(this->dialogType)
					{
						case chooserDialogType::saveDialog:
							{
								QString tp;
								if(this->fileNameEdit.text().isEmpty()==true)
									return;

								tp=QString("%1/%2").arg(this->startDir).arg(this->fileNameEdit.text());
								if((QFileInfo(tp).exists()==true) && (this->overwriteWarning==true) && (this->dialogType==chooserDialogType::saveDialog))
									{
										QMessageBox::StandardButton reply=QMessageBox::question(&this->dialogWindow,"File exists","File exists! Overwrite?",QMessageBox::Yes|QMessageBox::No);
										if(reply==QMessageBox::No)
											return;
									}
								this->setFileData();
								this->dialogWindow.hide();
							}
						break;
					case chooserDialogType::loadDialog:
						if(this->fileNameEdit.text().isEmpty()==false)
							{
								this->setFileData();
								this->dialogWindow.hide();
							}
						break;
					case chooserDialogType::folderDialog:
						this->setFileData();
						this->dialogWindow.hide();
						break;
				}
			}
		return;//enum class chooserDialogType{saveDialog,loadDialog,folderDialog};

			if(this->fileNameEdit.text().isEmpty()==true)
				{
					if(this->getFolder==true)
						{
							this->setFileData();
							this->dialogWindow.hide();
						}
					return;
				}

			if(QFileInfo(this->selectedFilePath).isDir()==true)
				{
					if(this->getFolder==true)
						{
							this->setFileData();
							this->dialogWindow.hide();
							return;
						}
					this->startDir=selectedFilePath;
					this->setFileList();
				}
			else
				{
					QString tp;
					if(this->fileNameEdit.text().isEmpty()==true)
						return;

					tp=QString("%1/%2").arg(this->startDir).arg(this->fileNameEdit.text());
					if((QFileInfo(tp).exists()==true) && (this->overwriteWarning==true) && (this->dialogType==chooserDialogType::saveDialog))
						{
							QMessageBox::StandardButton reply=QMessageBox::question(&this->dialogWindow,"File exists","File exists! Overwrite?",QMessageBox::Yes|QMessageBox::No);
							if(reply==QMessageBox::No)
								return;
						}
					this->setFileData();
					this->dialogWindow.hide();
				}
		});

	hlayout->addWidget(cancel);
	hlayout->addStretch();
	hlayout->addWidget(hidden);
	hlayout->addStretch();
	hlayout->addWidget(newfolder);
	hlayout->addStretch();
	hlayout->addWidget(refresh);
	hlayout->addStretch();
	hlayout->addWidget(apply);
	controlsvlayout->addLayout(hlayout);
	windowvlayout->addLayout(controlsvlayout);

	this->dialogWindow.setLayout(windowvlayout);
	this->setSideList();
	this->setFileList();
	if(this->dialogType!=chooserDialogType::saveDialog)
		this->fileNameEdit.setText("");
	else
		{
			this->selectedFilePath=this->startDir+"/"+this->saveName;
			this->selectedFileName=this->saveName;
			this->fileNameEdit.setText(this->saveName);
		}
	this->fileList.setDragEnabled(true);
	this->sideList.setAcceptDrops(true);
}

void chooserDialogClass::setMaxRecents(int maxrecents)
{
	this->maxRecents=maxrecents+1;
}

chooserDialogClass::chooserDialogClass(chooserDialogType type,QString name,QString startfolder)
{
	QSettings	prefs("KDHedger","ChooserDialog");
	QDir			folders("/");
	QString		command;
	QRect		r;

	this->recentFoldersPath=QString("%1/.config/KDHedger/recentfolders").arg(QDir::homePath());
	this->recentFilesPath=QString("%1/.config/KDHedger/recentfiles").arg(QDir::homePath());
	folders.mkpath(this->recentFoldersPath);
	folders.mkpath(this->recentFilesPath);

	if(startfolder.isEmpty()==true)
		{
			if(this->dialogType==chooserDialogType::saveDialog)
				this->startDir=prefs.value("lastsavefolder").toString();
			else
				this->startDir=prefs.value("lastloadfolder").toString();
		}
	else
		this->startDir=startfolder;

	this->dialogType=type;
	if(type==chooserDialogType::saveDialog)
		{
			if(name.isEmpty()==false)
				this->saveName=name;
			else
				this->saveName="Untitled";
			this->fileNameEdit.setText(this->saveName);
		}

	if(type==chooserDialogType::loadDialog)
		{
			this->saveName="";
		}

	if((this->startDir.isEmpty()==true) || (QFileInfo(this->startDir).exists()==false))
		this->startDir="/";

	this->startDir=QDir::cleanPath(this->startDir);
	this->setFolderPathsDrop();

	this->buildMainGui();

	r=prefs.value("size",QVariant(QRect(50,50,800,600))).value<QRect>();
	this->dialogWindow.setGeometry(r);

	command=QString("pushd %1/ >/dev/null;ls -t1|tail -n +%2| xargs -I {} rm '{}';popd >/dev/null").arg(this->recentFilesPath).arg(this->maxRecents);
	system(command.toStdString().c_str());
	command=QString("pushd %1 >/dev/null;ls -t1|tail -n +%2| xargs -I {} rm '{}';popd >/dev/null").arg(this->recentFoldersPath).arg(this->maxRecents);
	system(command.toStdString().c_str());
}
