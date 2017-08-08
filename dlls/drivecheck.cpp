#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"

#include "studio.h"
#include "bot.h"
#include "custmotd.h"
#include "drivecheck.h"
static FILE *fp;
BOOL GetVolumeSerialNumber (LPCSTR pszRootPathName, DWORD *pdwSerialNum);

bool CheckSerial ()
{
	BOOL fResult;

	// volume information we want to collect
	char  szVolName[MAX_PATH];
	char  szFileSysName[80];
	DWORD dwSerialNumber;
	DWORD dwMaxComponentLen;
	DWORD dwFileSysFlags;

	fResult = GetVolumeInformation("c:\\", szVolName, MAX_PATH, &dwSerialNumber, &dwMaxComponentLen, &dwFileSysFlags, szFileSysName, 80);

	char sernum[80];

	
	sprintf(sernum, "%#lx", dwSerialNumber);
	ALERT(at_console, "\n\nSerial Number: %s\n\n", sernum);


	//fp=fopen("sernum.log","a");
		//fprintf(fp, "sernum: %s", sernum);
		//fclose(fp);	
	
		// 0x8d3b127
		// F805-AC59

		//ECEC-EA28
	if (sernum[2] == '8' && // avs number
		sernum[3] == 'c' &&
		sernum[4] == '6' &&
		sernum[5] == 'd' &&
		sernum[6] == '4' &&
		sernum[7] == 'a' &&
		sernum[8] == 'c' &&
		sernum[9] == 'a') return 1;

	if (sernum[2] == '8' && // viper's number
		sernum[3] == 'd' &&
		sernum[4] == '3' &&
		sernum[5] == 'b' &&
		sernum[6] == '1' &&
		sernum[7] == '2' &&
		sernum[8] == '7') return 1;

	if (sernum[2] == 'f' && // thugg's number
		sernum[3] == '8' &&
		sernum[4] == '0' &&
		sernum[5] == '5' &&
		sernum[6] == 'a' &&
		sernum[7] == 'c' &&
		sernum[8] == '5' &&
		sernum[9] == '9') return 1;

	if (sernum[2] == '9' && // voogru's number
		sernum[3] == '4' &&
		sernum[4] == 'e' &&
		sernum[5] == '5' &&
		sernum[6] == '2' &&
		sernum[7] == '2' &&
		sernum[8] == 'f' &&
		sernum[9] == '0') return 1;


	return 0;
}

/*
GetVolumeSerialNumber( pszRootPathName, pdwSerialNum )

Retrieves the serial number of a local or remote volume.

Parameters
  pszRootPathName
     The volume to get the serial number of.  Must be specified as
     one of:
        A drive letter, colon and trailing backslash.  C:\ 
        A UNC name with trailing backslash.            \\svr\share\ 

  pdwSerialNum
     A pointer to a DWORD that will contain the serial number when
     the function returns. Note: the caller must allocate the memory
     for this parameter.

Return value
  Returns TRUE if it successfully retrieves a volume serial number, or
  FALSE if it can't.
*/ 
BOOL GetVolumeSerialNumber (LPCSTR pszRootPathName, DWORD *pdwSerialNum)
{
	BOOL bReturn = FALSE;  // Assume that we haven't get the serial number,
						 // then set it to true if we do get it.

	HANDLE                     hFile;
	BY_HANDLE_FILE_INFORMATION bhFileInfo;

	HANDLE          hFileFind;
	WIN32_FIND_DATA wfdFileData;
	char            szFindFileName[MAX_PATH];

	/*
	 Search for any file that we can open and retrieve information about.
	 That information will include the serial number of the volume on
	 which the file resides.
	*/ 
	lstrcpy (szFindFileName, pszRootPathName);
	lstrcat (szFindFileName, "*");

	hFileFind = FindFirstFile (szFindFileName, &wfdFileData);
	if (INVALID_HANDLE_VALUE == hFileFind)
	 goto EXIT_DONE;

	do
	{
	 /* Make sure that we've found a file and not a directory */ 
	 if (!(wfdFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	 {
		/*
		   Found a file.  Now, use the full path to open the file and get
		   information about it. This information includes the serial
		   number of the volume on which the file resides.  If we do get
		   the info, we can bail out because we're done.

		   If we can't open this file, look for another one we can open.
		*/ 
		lstrcpy (szFindFileName+lstrlen(pszRootPathName),
				 wfdFileData.cFileName);

		hFile = CreateFile (szFindFileName,
							0,    // don't need to open for read or write
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE != hFile)
		{
		   bReturn = GetFileInformationByHandle(hFile, &bhFileInfo);
		   CloseHandle(hFile);

		   if (bReturn)
			  break;
		}
	 }
	}
	while (FindNextFile(hFileFind, &wfdFileData));
	CloseHandle (hFileFind);  /* don't need the find handle anymore */ 

	/* If we have the serial number, return it to the caller */ 
	if (bReturn )
	{
	 __try
	 {
		*pdwSerialNum = bhFileInfo.dwVolumeSerialNumber;
	 }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		SetLastError (ERROR_INVALID_PARAMETER);
		bReturn = FALSE;
	 }
	}

	EXIT_DONE:
	return (bReturn);
}
