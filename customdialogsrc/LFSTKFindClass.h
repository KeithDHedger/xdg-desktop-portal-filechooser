/*
 *
 * ©K. D. Hedger. Wed 20 Nov 14:36:39 GMT 2024 keithdhedger@gmail.com

 * This file (LFSTKFindClass.h) is part of xdg-desktop-portal-filechooser.

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

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <ftw.h>
#include <functional>
#include <string.h>

#include "LFSTKUtilityClass.h"

#ifndef _FINDCLASS_
#define _FINDCLASS_

enum {BROKENLINKTYPE=0,ANYTYPE,FILELINKTYPE,FILETYPE,FOLDERLINKTYPE,FOLDERTYPE};

struct	dataStruct
{
	std::string name;
	std::string path;
	std::string	namesanssuffix;
	int			fileType;
};

/**
 * \brief Find files etc.
*/
class LFSTK_findClass
{
	public:
		~LFSTK_findClass();
		LFSTK_findClass();

		void			LFSTK_setDepth(int min,int max);
		void			LFSTK_setFindType(int type);
		int			LFSTK_getFindType(void);
		void			LFSTK_setFollowLinks(bool follow);
		bool			LFSTK_getFollowlinks(void);
		void			LFSTK_setIncludeHidden(bool hidden);
		bool			LFSTK_getIncludeHidden(void);
		void			LFSTK_setFullPath(bool usefull);
		bool			LFSTK_getFullPath(void);
		void			LFSTK_setSortDecending(bool down);
		bool			LFSTK_getSort(void);
		void			LFSTK_setIgnoreBroken(bool ignore);
		bool			LFSTK_getIgnoreBroken(void);
		void			LFSTK_setFileTypes(std::string suffix);
		std::string	LFSTK_getFileTypes(void);
		void			LFSTK_setIgnoreNavLinks(bool ignore);
		bool			LFSTK_getIgnoreNavLinks(void);
		void			LFSTK_setNameFilter(std::string name);
		void			LFSTK_setIgnoreFolders(bool ignore);
		bool			LFSTK_getIgnoreFolderss(void);

		void			LFSTK_findFiles(const char *dir,bool multi=false);
		void			LFSTK_sortCaseSensitive(bool casesensitive);
		void			LFSTK_sortByName(void);
		void			LFSTK_sortByPath(void);
		void			LFSTK_sortByType(void);
		void			LFSTK_sortByTypeAndName(void);
		int			LFSTK_getDataCount(void);
		int			LFSTK_getMinDepth(void);
		int			LFSTK_getMaxDepth(void);
		dataStruct	*LFSTK_findNamed(std::string name,std::string suffix="");

		std::vector<dataStruct> data;
		bool		caseSensitive=true;


		void		deleteData(void);





	private:
		bool		fileTypeTest(int filetype);

		bool		ignoreFolders=false;
		int		minDepth=-1;
		int		maxDepth=10000;
		int		findType=ANYTYPE;
		bool		followLinks=true;
		bool		includeHidden=false;
		bool		fullPath=false;
		bool		sortDescending=false;
		bool		ignoreBroken=false;
		std::string	fileTypes;
		std::string	nameFilter;
		bool		ignoreNavLinks=false;

		int		getRealType(std::string path);
		void		moveNavItem(void);
};

#endif
