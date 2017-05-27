#include "filehandling.h"

BOOLEAN lookup(int id,const char* file_name) {
    DIR* dir;
    struct dirent* entry;
    char dir_name[15];
    sprintf(dir_name,"%d_files%c",id,'\0');
    CHECK((dir = opendir(dir_name)));
    CHECK((entry = readdir(dir)));
    do { 
    	if(!strcmp(entry->d_name,file_name)){
    		closedir(dir);
    		return TRUE;
    	}
    } while ((entry = readdir(dir)));
    closedir(dir);
    return FALSE;
}


int send_file(SOCKET receiver, const char* file, int id) {
    char file_name[50];
    sprintf(file_name,"%d_files/%s%c",id,file,'\0');
	int fd = open(file_name, O_RDONLY, 0777);
	CHECK(fd != -1);
	char buffer[BUF_SIZE];
	memset(buffer, 0, BUF_SIZE);	
	while(read(fd, buffer, BUF_SIZE) != 0) {
		CHECK(write(receiver, buffer, BUF_SIZE) != -1);
		memset(buffer,' ', BUF_SIZE);
	}	
	write(receiver, END,BUF_SIZE);
	close(fd);
	return EXIT_SUCCESS;
}


int rcv_file(SOCKET sender, const char* file,int id) {
    char file_name[50];
    sprintf(file_name,"%d_files/%s%c",id,file,'\0');

	int fd = open(file_name,O_WRONLY | O_CREAT | O_TRUNC, 0777);
	CHECK(fd != -1);
	char buffer[BUF_SIZE];

	while(TRUE) {
		memset(buffer, 0, BUF_SIZE);
		read(sender, buffer, BUF_SIZE);
		if(!strcmp(buffer,END)) {
			close(fd);
			return EXIT_SUCCESS;
		}
		write(fd, buffer,BUF_SIZE);
	}
	close(fd);
	return EXIT_SUCCESS;
}

void rm_shared_dir(int id) {
	char path[50];
	sprintf(path,"%d_files%c",id,'\0');
	rm_shared_dir2(path);
}

int rm_shared_dir2(char* path) {	
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;
    if (d) {
    	struct dirent *p;
    	r = 0;
    	while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;
          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);
          if (buf) {
             struct stat statbuf;
             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = rm_shared_dir2(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
    	}
      closedir(d);
    }
    if (!r)
      r = rmdir(path);
 	return r;
}


void create_shared_dir(int id) {
	char dir_name[15];
	sprintf(dir_name,"%d_files%c",id,'\0');
	struct stat st = {0};

	if(stat(dir_name, &st) == -1)
		mkdir(dir_name,0777);
}






