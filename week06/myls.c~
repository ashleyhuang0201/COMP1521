// myls.c ... my very own "ls" implementation

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
# include <bsd/string.h>
#endif
#include <sysexits.h>
#include <unistd.h>

#define MAXDIRNAME 256
#define MAXFNAME 256
#define MAXNAME 24

char *rwxmode (mode_t, char *);
char *username (uid_t, char *);
char *groupname (gid_t, char *);

int main (int argc, char *argv[])
{
	// string buffers for various names
	char uname[MAXNAME+1]; // UNCOMMENT this line
	char gname[MAXNAME+1]; // UNCOMMENT this line
	char mode[MAXNAME+1]; // UNCOMMENT this line

	// collect the directory name, with "." as default
	char dirname[MAXDIRNAME] = ".";
	if (argc >= 2)
		strlcpy (dirname, argv[1], MAXDIRNAME);

	// check that the name really is a directory
	struct stat info;
	if (stat (dirname, &info) < 0)
		err (EX_OSERR, "%s", dirname);

	if (! S_ISDIR (info.st_mode)) {
		errno = ENOTDIR;
		err (EX_DATAERR, "%s", dirname);
	}

	// open the directory to start reading
	// ... TODO ...
	DIR *df;
	df = opendir(dirname);
	
	// struct entry to read directory entries
	struct dirent *entry; // UNCOMMENT this line
	// ... TODO ...

	//display file or file system status 
	struct stat ent;

	//dirname, /, entry->d_name
	//strcat -  joins source and destination 

	chdir(dirname);

	//int chdir(const char *path)
	//function makes directory dirname to become the current working directory

	//while there are more entries 
	while ((entry = readdir(df)) != NULL) {
		//ignore the object if its name starts with '.'
		//man readdir ~ d_name[256] = filename
		if (entry->d_name[0] != '.') {
			//get the struct stat info for the object using its path
			lstat(entry->d_name, &ent);
			rwxmode(ent.st_mode, mode);
			username(ent.st_uid, uname);
			groupname(ent.st_gid, gname);
			//print the details using the object name
			//and the struct status info
			//minimum number of digits that has to appear
			printf("                             %s %-8.8s %-8.8s %8lld %s\n", mode, uname, gname, (long long) ent.st_size, entry->d_name);
		}
	}


	// finish up
	closedir(df); // UNCOMMENT this line

	return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode (mode_t mode, char *str)
{
	//For the type of the file, first character
	//S_IFMT is the mask for the file type
	switch(mode & S_IFMT) {
		//for regular file
		case S_IFREG:
			str[0] = '-';
			break;
		//for the directory 
		case S_IFDIR:
			str[0] = 'd';
			break;

		//for symbolic link
		case S_IFLNK:
			str[0] = 'l';
			break;
		
		default:
			str[0] = '?';
			break;

 	}
	//For the user permission

	//S_IRWXU mask for file owner permission
	//if the owner has the read permission
	if ((mode & S_IRWXU) & S_IRUSR) {
		str[1] = 'r';
	} else {
		str[1] = '-';
	}

	//if the owner has write permission
	if ((mode & S_IRWXU) & S_IWUSR) {
		str[2] = 'w';
	} else {
		str[2] = '-';
	}

	//if the owner has execute permission
	if ((mode & S_IRWXU) & S_IXUSR) {
		str[3]='x';
	} else {
		str[3] = '-';
	}

	//For the file group permission
	//S_IRWXG mask for file group permission
	//if the group has read permission

	if ((mode & S_IRWXG) & S_IRGRP) {
		str[4]='r';
	} else {
		str[4] = '-';
	}
	
	//if group has write permission
	if ((mode & S_IRWXG) & S_IWGRP) {
		str[5] = 'w';
	
	} else {
		str[5] = '-';
	}

	//if group has execute permission
	if ((mode & S_IRWXG) & S_IXGRP) {
		str[6] = 'x';
	} else {
		str[6] = '-';
	}

	//if others have write permission
	if ((mode & S_IRWXO) & S_IWOTH) {
		str[8] = 'w';

	} else {
		str[8] = '-';

	}
	
	//if others have execute permission
	if ((mode & S_IRWXO) & S_IXOTH) {
		str[9] = 'x';

	} else {
		str[9] = '-';
	}

	//Terminate the string 
	str[10] = '\0';
	
	return str;
}

// convert user id to user name
char *username (uid_t uid, char *name)
{
	struct passwd *uinfo = getpwuid (uid);
	if (uinfo != NULL)
		snprintf (name, MAXNAME, "%s", uinfo->pw_name);
	else
		snprintf (name, MAXNAME, "%d?", (int) uid);
	return name;
}

// convert group id to group name
char *groupname (gid_t gid, char *name)
{
	struct group *ginfo = getgrgid (gid);
	if (ginfo != NULL)
		snprintf (name, MAXNAME, "%s", ginfo->gr_name);
	else
		snprintf (name, MAXNAME, "%d?", (int) gid);
	return name;
}
