
#ifndef _CHOOSERDIALOG_
#define _CHOOSERDIALOG_

#include "globals.h"

#define MAXIMAGESIZETOTHUMB 2000000

enum class chooserDialogType{saveDialog,loadDialog,folderDialog};

class chooserDialogClass
{
	public:
		chooserDialogClass(chooserDialogType type,QString savename="Untitled",QString startfolder="");
		~chooserDialogClass();

		QDialog				dialogWindow;
		QVector<QString>		multiFileList;
		bool					valid=false;

		void					setShowImagesInList(bool show=false);
		void					setMultipleSelect(bool select);
		void					addFileTypes(QString types);




QLineEdit			filepathEdit;

	private:

//main
		QString				selectedFolderPath="";
		QString				currentFolderPath="/";
		QComboBox			*folderCombo=NULL;
		QPushButton			*apply=NULL;

		QListView			fileList;
		QStandardItemModel	*fileListModel;
		QListView			sideList;
		QStandardItemModel	*sideListModel;

		QComboBox			fileTypes;

		QLabel				previewIcon;
		QLabel				previewMimeType;
		QLabel				previewSize;
		QLabel				previewMode;

		bool					useMulti=false;

		chooserDialogType	dialogType=chooserDialogType::loadDialog;
		void					buildMainGui(void);
		void					showPreViewData(QString file);
		void					doChoose(void);
		void					setFavs(void);
		void					setExitData(bool valid);
		void					getFilePermissions(QString filePath);
		void					fileEntryTextEdited(QString text);

//sidlist cbs
		QString				recentFoldersPath;
		QString				recentFilesPath;
		int					maxRecents=21;

		void					setSideList(void);
		void					selectSideItem(const QModelIndex &index);
		void					doubleClickSideList(const QModelIndex &index);

//filelist cbs
		bool					showHidden=false;
		bool					showThumbsInList=false;
		bool					fromRecents=false;

		QIcon				getFileIcon(QString path);
		void					doubleClickFileList(const QModelIndex &index);
		void					fileListSelectionChanged(void);
		void					setSelectedFiles(const QModelIndex &index,bool clear=false);
		void					setFileList(QString dir,QDir::SortFlags sortas=QDir::Name);
};

#endif
