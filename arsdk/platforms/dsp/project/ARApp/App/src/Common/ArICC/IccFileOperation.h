#ifndef ICCFILEOPERATION_H
#define ICCFILEOPERATION_H
#ifdef __cplusplus
extern "C" {
#endif

#define ONLY_FS_OPERATION (1)
#define O_RDWR 00000002
#define O_CREAT 00000100
#define O_RDONLY 00000000

#define MSG_CEVA_WITH_LINUX_FOPS_0  0xff001234

/*
 dsp file system initialize
 para       none
 return     when init error will return a negative number
 	 	    0 stands for init successfully
 */
extern int xArFsInit();

/*
 open a file
 para[in]   file_name  : the file path
 para[in]   flag       : open method
 return     when open error will return a negative number
			when open successfully,file descriptor will be returned
 */
extern int xArOpen(char * file_name,int flag);

/*
 get file length
 para[in]       fd          : file descriptor
 return     when read error will return a negative number
			otherwise returns file length
 */
extern int xArGetFileLen(int fd);

/*
 read a file
 para[in]       fd          : file descriptor
 para[in]       buf         : buffer to fill read content
 para[in,out]   read_size   : file size want to read.If you want to
 	 	 	 	 	 	 	  read entire file,fill zero
 return     when read error will return a negative number
			0 stands for reading successfully
 */
extern int xArRead(int fd, char *buf, int * read_size);

/*
 write a file
 para[in]       fd          : file descriptor
 para[in]       buf         : buffer to fill write content
 para[in,out]   write_size   : file size want to write
 return     when read error will return a negative number
			0 stands for reading successfully
 */
extern int xArWrite(int fd, char *buf, int *write_size);
/*
 close a file
 para[in]       fd          : file descriptor
 return     when close error will return a negative number
			0 stands for close successfully
 */
extern int xArClose(int fd);

/*wait linux file system, icc, pthread ready*/
extern void vWaitLinuxFsReady();
#ifdef __cplusplus
}
#endif
#endif /* AR_DSP_COMMON_API_H */
