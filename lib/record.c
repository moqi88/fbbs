#include "bbs.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define BUFSIZE (MAXUSERS + 244)

USE_TRY;

//	循环写字符到文件，直到写入size个字节
//	origsz,bp是多余的？
static int safewrite(int fd, const void *buf, int size)
{
	int cc, sz = size, origsz = size;
	const void *bp = buf;

	do {
		cc = write(fd, bp, sz);
		if ((cc < 0) && (errno != EINTR)) {
			report("safewrite err!", "");
			return -1;
		}
		if (cc > 0) {
			bp += cc;
			sz -= cc;
		}
	} while (sz > 0);
	return origsz;
}

//	若文件filename不存在,返回-1
//	否则返回filename中存放的记录数
long get_num_records(const char *filename, const int size)
{
	struct stat st;
	if (stat(filename, &st) == -1)
		return 0;
	return (st.st_size / size);
}

//增加一个记录，大小为size,首地址为record
//	文件名为filename
int append_record(const char *filename, const void *record, int size)
{
	int fd;
	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1) {
		report("open file error in append_record()", "");
		return -1;
	}
	FLOCK(fd, LOCK_EX);
	lseek(fd, 0, SEEK_END);
	if (safewrite(fd, record, size) == -1)
		report("apprec write err!", "");
	FLOCK(fd, LOCK_UN);
	close(fd);
	return 0;
}

//取得记录的句柄,并存放在rptr中
//	fd是文件的描述符,size表示记录的大小,id表示记录的位置
//	不成功时,返回-1 ; 成功时,返回0
static int get_record_handle(int fd, void *rptr, int size, int id)
{
	if (lseek(fd, size * (id - 1), SEEK_SET) == -1)
		return -1;
	if (read(fd, rptr, size) != size)
		return -1;
	return 0;
}

//取得记录,filename表示文件名,其它参数见get_record_handle
int get_record(char *filename, void *rptr, int size, int id)
{
	int fd;
	int ret;

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	ret = get_record_handle(fd, rptr, size, id);
	close(fd);
	return ret;
}

//在文件filename中的第id-1个记录处读取大小为size,数量为number的记录集
//	如果失败,返回-1,如果未能读取number个记录,则返回读取的记录数
int get_records(const char *filename, void *rptr, int size, int id,
		int number)
{
	int fd;
	int n;
	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1) {
		close(fd);
		return 0;
	}
	if ((n = read(fd, rptr, size * number)) == -1) {
		close(fd);
		return -1;
	}
	close(fd);
	return (n / size);
}

#ifndef THREAD_C

//	对名为filename的记录文件执行fptr函数
int apply_record(char *filename, APPLY_FUNC_ARG fptr, int size, void *arg,
		int applycopy, int reverse)
{
	void *buf, *buf1, *buf2 = NULL;
	int i, fd;
	size_t file_size;
	int count;

	BBS_TRY {
		// TODO: is nolock safe here?
		if ((fd = mmap_open(filename, MMAP_NOLOCK, &buf, &file_size)) < 0)
			BBS_RETURN(0);
		count = file_size / size; //记录的数目
		if (reverse)
		buf1 = buf + (count - 1) * size;
		else
		buf1 = buf;
		for (i = 0; i < count; i++) {
			if (applycopy) {
				buf2 = malloc(size);
				memcpy(buf2, buf1, size);
			} else {
				buf2 = buf1;
			}
			if ((*fptr) (buf2, reverse ? count - i : i + 1, arg) == QUIT) {
				//执行函数fptr,buf为缓冲区首地址,arg为第三参数
				mmap_close((void *) buf, file_size, fd); //终止内存映射,本来没加锁,
				//现在不需要解锁
				if (applycopy)
				free(buf2);
				BBS_RETURN(QUIT);
			}
			if (reverse)
			buf1 -= size;
			else
			buf1 += size;
		}
	}
	BBS_CATCH {
	}
	BBS_END mmap_close(buf, file_size, -1);

	if (applycopy)
		free(buf2);
	return 0;
}

/*---   End of Addition     ---*/
/* search_record进行了预读优化,以减少系统调用次数,提高速度. ylsdd, 2001.4.24 */
/* COMMAN : use mmap to improve search speed */
//	在filename文件中搜索 比较函数为fptr,欲搜索记录为farg
//	搜索长度为O(n)可以考虑改进
int search_record(char *filename, void *rptr, int size,
		RECORD_FUNC_ARG fptr, void *farg)
{
	int i, fd;
	void *buf, *buf1;
	size_t filesize;

	BBS_TRY {
		// TODO: is nolock safe here?
		if ((fd = mmap_open(filename, MMAP_NOLOCK, &buf, &filesize)) < 0)
			BBS_RETURN(0);
		for (i = 0, buf1 = buf; i < filesize / size; i++, buf1 += size) {
			if ((*fptr) (farg, buf1)) {
				if (rptr)
					memcpy(rptr, buf1, size);
				mmap_close(buf, filesize, -1);
				BBS_RETURN(i + 1);
			}
		}
	}
	BBS_CATCH {
	}
	BBS_END mmap_close(buf, filesize, -1);

	return 0;
}

//	将filename文件第id个记录替换为rptr所指向的数据
int substitute_record(char *filename, void *rptr, int size, int id)
{
	/*     * add by KCN      */
	struct flock ldata;
	int retval;
	int fd;

	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1)
		return -1;
	/*
	 * change by KCN
	 * flock(fd,LOCK_EX) ;
	 */
	ldata.l_type = F_WRLCK;
	ldata.l_whence = 0;
	ldata.l_len = size;
	ldata.l_start = size * (id - 1);
	if ((retval = fcntl(fd, F_SETLKW, &ldata)) == -1) {//以互斥方式锁文件
		//bbslog("user", "%s", "reclock error");
		close(fd);
		/*---	period	2000-10-20	file should be closed	---*/
		return -1;
	}
	if (lseek(fd, size * (id - 1), SEEK_SET) == -1) { //无法到文件的指定位置
		// bbslog("user", "%s", "subrec seek err");
		/*---	period	2000-10-24	---*/
		ldata.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &ldata);
		close(fd);
		return -1;
	}
	//由infotech修改,去掉safewrite的判断,以后调试时可以再增加
	//if (safewrite(fd, rptr, size) != size)
	safewrite(fd, rptr, size);
	//bbslog("user", "%s", "subrec write err");
	/*
	 * change by KCN
	 * flock(fd,LOCK_UN) ;
	 */

	ldata.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &ldata);
	close(fd);

	return 0;
}

//from smthbbs, 2003.10.26
int delete_record(char *filename, int size, int id,
		RECORD_FUNC_ARG filecheck, void *arg)
{
	int fdr;
	size_t filesize;
	void *ptr;
	int ret;

	if (id <= 0)
		return 0;
	BBS_TRY {
		if ((fdr = mmap_open(filename, MMAP_RDWR, &ptr, &filesize)) < 0)
			BBS_RETURN(-1);
		ret = 0;
		if (id * size> filesize) {
			ret = -2;
		} else {
			if (filecheck) { //检查filecheck函数是否存在?
				if (!(*filecheck) (ptr + (id - 1) * size, arg)) {
					for (id = 0; id * size < filesize; id++)
					if ((*filecheck) (ptr + (id - 1) * size, arg))
					break;
					if (id * size >= filesize)
					ret = -2;
				}
			}
		}
		if (ret == 0) {
			memcpy(ptr + (id - 1) * size, ptr + id * size, filesize - size * id);
			// 将被删除记录后所有记录向前移动一个位置,
			ftruncate(fdr, filesize - size);
			// 将文件截短一个记录
		}
	}
	BBS_CATCH {
		ret = -3;
	}
	BBS_END mmap_close(ptr, filesize, fdr);

	return ret;
}

//插入记录
int insert_record(char *filename, int size, RECORD_FUNC_ARG filecheck,
		void *arg)
{
	char *ptr, *rptr;
	int fdr;
	size_t filesize;
	int ret;

	if (!filecheck||!arg)
		return -1;
	fdr=open(filename, O_RDWR, 0600);
	lseek(fdr, 0, SEEK_END);
	write(fdr, arg, size);

	BBS_TRY {
		if (safe_mmapfile_handle( fdr, O_RDWR, PROT_READ | PROT_WRITE,
						MAP_SHARED, (void **) &ptr, &filesize
				)
				== 0
		)
		BBS_RETURN(-1);
		ret = 0;
		rptr = ptr;
		filesize -= size;
		while(filesize) {
			if(filecheck(rptr, arg)) {
				break;
			}
			rptr += size;
			filesize-= size;
		}
		if(filesize) {
			memmove(rptr + size, rptr, filesize);
			memcpy(rptr, arg, size);
		}
	}
	BBS_CATCH {
		ret = -3;
	}
	BBS_END mmap_close(ptr, filesize, fdr);
	return ret;
}

#endif
