#include "syscall.h"
#include "drivers/ext2/ext2.h"
#include "interrupts/idt.h"
#include "signal.h"
#include "usertask/Task.h"
#include "stdlib/string.h"
#include <sys/syscall.h>
#include "schedule/Scheduler.h"

#define ARG1(r) r->rdi
#define ARG2(r) r->rsi
#define ARG3(r) r->rdx
#define RETURN(r) r->rax=

int ioctl(int fd, int op,...);

char* SYSCALL_GET_NAME(int v);
void syscall(registers* r){
	//printf("PROCESSING SYSCALL [%s]\n",SYSCALL_GET_NAME(r->rax));
	
	//TASK* t=TaskCurrent();
	//memcpy(t->r,r,sizeof(registers));
	Process* proc=getProcess();
	if(proc!=NULL){
		memcpy(&proc->r,r,sizeof(registers));
	}
	int ret=0;
	switch(r->rax){
		case SYS_exit:
			exit(r,ARG1(r));
			break;
		case SYS_write:
			ret=write(ARG1(r),(void*)ARG2(r),ARG3(r));
			break;
		case SYS_writev:
			ret=writev(ARG1(r),(void*)ARG2(r),ARG3(r));
			break;
		case SYS_open:
			ret=open((void*)ARG1(r),ARG2(r),ARG3(r));
			break;
		case SYS_read:
			ret=read(ARG1(r),(void*)ARG2(r),ARG3(r));
			break;
		case SYS_execve:
			ret=execve((void*)ARG1(r),(void*)ARG2(r),(void*)ARG3(r));
			break;
		case SYS_getpid:
			ret=getpid();
			break;
		case SYS_fork:

			// TASK* t=TaskCurrent();
			// if(t!=NULL){
			// 	printf("__RIP:\t%p\n",r->rip);
			// 	t->r->rip=r->rip;
			// }
			ret=fork(r);
			break;
		case SYS_stat:
			ret=fstat(ARG1(r),(void*)ARG2(r));
			break;
		case SYS_dup:
			ret=dup(ARG1(r));
			break;
		case SYS_dup2:
			ret=dup2(ARG1(r),ARG2(r));
			break;
		case SYS_close:
			ret=close(ARG1(r));
			break;
		case SYS_fcntl:
			ret=fcntl(ARG1(r),ARG2(r),ARG3(r));
			break;
		case SYS_ioctl:
			ret=ioctl(ARG1(r),ARG2(r));
			break;
		case SYS_rt_sigprocmask:
			ret=sigprocmask(ARG1(r), (const sigset_t *)ARG2(r),(sigset_t *)ARG3(r));
			//exit(r,-1);
			break;
		default:
			printf("UNRECOGNISED SYSCALL [0x%x][%s]\n",r->rax,SYSCALL_GET_NAME(r->rax));
			kprintf("UNRECOGNISED SYSCALL [0x%x][%s]\n",r->rax,SYSCALL_GET_NAME(r->rax));
			exit(r,-1);
	}
	RETURN(r) ret;
}
int FD_FIND(){
	Process* proc=getProcess();
	int idx=-1;
	for(int i=3;i<256;i++){
		if(proc->fd[i].used!=1){
			idx=i;
			break;
		}
	}
	return idx;
}
int ioctl(int fd, int op,...){
	printf("IOCTL ON FD(%d) WITH OP [%x] WITH PARAMS[",fd,op);

    va_list args;
    va_start(args, 1);  
    for (int i = 0; i < 1; i++) 
        printf("%d ", va_arg(args, int));
    printf("]\n");
    va_end(args);
	return 0;
}
int fstat(int fd, struct stat *statbuf){
	Process* t=getProcess();
	return FSTAT(&t->fd[fd].file,statbuf);
}
int getpid(){
	return TaskCurrent()->id;
}

int fcntl(int fd, int op, int args /* arg */ ){
	printf("FCNTL ON FD(%d) WITH OP [%d] AND {%d} \n",fd,op,args);
	return 1;
}
int close(int fd){
	Process* t=getProcess();
	if(t->fd[fd].used==0)return 0;
	FILE_DESC_FREE(&t->fd[fd]);
	return 1;
}
int dup(int fd){
	Process* t=getProcess();
	if(t->fd[fd].used==0)return 0;
	int idx=FD_FIND();
	FILE_DESC_DUP(&t->fd[idx],&t->fd[fd]);
	return 1;
}
int dup2(int fd,int _new){
	Process* t=getProcess();
	if(t->fd[fd].used==0)return 0;
	close(_new);
	FILE_DESC_DUP(&t->fd[_new],&t->fd[fd]);
	return 1;
}
int open(const char* path,int flags,umode_t mode){
	Process* t=getProcess();
	int idx=FD_FIND();
	if(idx==-1)return 0;
	FILE_DESC* emptyfd=&t->fd[idx];
	FILE f;
	if(FILE_GET(emptyfd,&f,path)){
		return idx;
	}else{
		return 0;
	}
	return 0;
}



char* SYSCALL_GET_NAME(int v){
#define MSG(x) if(x==v) return #x;

	MSG(SYS_read)
		MSG(SYS_write)
		MSG(SYS_open)
		MSG(SYS_close)
		MSG(SYS_stat)
		MSG(SYS_fstat)
		MSG(SYS_lstat)
		MSG(SYS_poll)
		MSG(SYS_lseek)
		MSG(SYS_mmap)
		MSG(SYS_mprotect)
		MSG(SYS_munmap)
		MSG(SYS_brk)
		MSG(SYS_rt_sigaction)
		MSG(SYS_rt_sigprocmask)
		MSG(SYS_rt_sigreturn)
		MSG(SYS_ioctl)
		MSG(SYS_pread64)
		MSG(SYS_pwrite64)
		MSG(SYS_readv)
		MSG(SYS_writev)
		MSG(SYS_access)
		MSG(SYS_pipe)
		MSG(SYS_select)
		MSG(SYS_sched_yield)
		MSG(SYS_mremap)
		MSG(SYS_msync)
		MSG(SYS_mincore)
		MSG(SYS_madvise)
		MSG(SYS_shmget)
		MSG(SYS_shmat)
		MSG(SYS_shmctl)
		MSG(SYS_dup)
		MSG(SYS_dup2)
		MSG(SYS_pause)
		MSG(SYS_nanosleep)
		MSG(SYS_getitimer)
		MSG(SYS_alarm)
		MSG(SYS_setitimer)
		MSG(SYS_getpid)
		MSG(SYS_sendfile)
		MSG(SYS_socket)
		MSG(SYS_connect)
		MSG(SYS_accept)
		MSG(SYS_sendto)
		MSG(SYS_recvfrom)
		MSG(SYS_sendmsg)
		MSG(SYS_recvmsg)
		MSG(SYS_shutdown)
		MSG(SYS_bind)
		MSG(SYS_listen)
		MSG(SYS_getsockname)
		MSG(SYS_getpeername)
		MSG(SYS_socketpair)
		MSG(SYS_setsockopt)
		MSG(SYS_getsockopt)
		MSG(SYS_clone)
		MSG(SYS_fork)
		MSG(SYS_vfork)
		MSG(SYS_execve)
		MSG(SYS_exit)
		MSG(SYS_wait4)
		MSG(SYS_kill)
		MSG(SYS_uname)
		MSG(SYS_semget)
		MSG(SYS_semop)
		MSG(SYS_semctl)
		MSG(SYS_shmdt)
		MSG(SYS_msgget)
		MSG(SYS_msgsnd)
		MSG(SYS_msgrcv)
		MSG(SYS_msgctl)
		MSG(SYS_fcntl)
		MSG(SYS_flock)
		MSG(SYS_fsync)
		MSG(SYS_fdatasync)
		MSG(SYS_truncate)
		MSG(SYS_ftruncate)
		MSG(SYS_getdents)
		MSG(SYS_getcwd)
		MSG(SYS_chdir)
		MSG(SYS_fchdir)
		MSG(SYS_rename)
		MSG(SYS_mkdir)
		MSG(SYS_rmdir)
		MSG(SYS_creat)
		MSG(SYS_link)
		MSG(SYS_unlink)
		MSG(SYS_symlink)
		MSG(SYS_readlink)
		MSG(SYS_chmod)
		MSG(SYS_fchmod)
		MSG(SYS_chown)
		MSG(SYS_fchown)
		MSG(SYS_lchown)
		MSG(SYS_umask)
		MSG(SYS_gettimeofday)
		MSG(SYS_getrlimit)
		MSG(SYS_getrusage)
		MSG(SYS_sysinfo)
		MSG(SYS_times)
		MSG(SYS_ptrace)
		MSG(SYS_getuid)
		MSG(SYS_syslog)
		MSG(SYS_getgid)
		MSG(SYS_setuid)
		MSG(SYS_setgid)
		MSG(SYS_geteuid)
		MSG(SYS_getegid)
		MSG(SYS_setpgid)
		MSG(SYS_getppid)
		MSG(SYS_getpgrp)
		MSG(SYS_setsid)
		MSG(SYS_setreuid)
		MSG(SYS_setregid)
		MSG(SYS_getgroups)
		MSG(SYS_setgroups)
		MSG(SYS_setresuid)
		MSG(SYS_getresuid)
		MSG(SYS_setresgid)
		MSG(SYS_getresgid)
		MSG(SYS_getpgid)
		MSG(SYS_setfsuid)
		MSG(SYS_setfsgid)
		MSG(SYS_getsid)
		MSG(SYS_capget)
		MSG(SYS_capset)
		MSG(SYS_rt_sigpending)
		MSG(SYS_rt_sigtimedwait)
		MSG(SYS_rt_sigqueueinfo)
		MSG(SYS_rt_sigsuspend)
		MSG(SYS_sigaltstack)
		MSG(SYS_utime)
		MSG(SYS_mknod)
		MSG(SYS_uselib)
		MSG(SYS_personality)
		MSG(SYS_ustat)
		MSG(SYS_statfs)
		MSG(SYS_fstatfs)
		MSG(SYS_sysfs)
		MSG(SYS_getpriority)
		MSG(SYS_setpriority)
		MSG(SYS_sched_setparam)
		MSG(SYS_sched_getparam)
		MSG(SYS_sched_setscheduler)
		MSG(SYS_sched_getscheduler)
		MSG(SYS_sched_get_priority_max)
		MSG(SYS_sched_get_priority_min)
		MSG(SYS_sched_rr_get_interval)
		MSG(SYS_mlock)
		MSG(SYS_munlock)
		MSG(SYS_mlockall)
		MSG(SYS_munlockall)
		MSG(SYS_vhangup)
		MSG(SYS_modify_ldt)
		MSG(SYS_pivot_root)
		MSG(SYS__sysctl)
		MSG(SYS_prctl)
		MSG(SYS_arch_prctl)
		MSG(SYS_adjtimex)
		MSG(SYS_setrlimit)
		MSG(SYS_chroot)
		MSG(SYS_sync)
		MSG(SYS_acct)
		MSG(SYS_settimeofday)
		MSG(SYS_mount)
		MSG(SYS_umount2)
		MSG(SYS_swapon)
		MSG(SYS_swapoff)
		MSG(SYS_reboot)
		MSG(SYS_sethostname)
		MSG(SYS_setdomainname)
		MSG(SYS_iopl)
		MSG(SYS_ioperm)
		MSG(SYS_create_module)
		MSG(SYS_init_module)
		MSG(SYS_delete_module)
		MSG(SYS_get_kernel_syms)
		MSG(SYS_query_module)
		MSG(SYS_quotactl)
		MSG(SYS_nfsservctl)
		MSG(SYS_getpmsg)
		MSG(SYS_putpmsg)
		MSG(SYS_afs_syscall)
		MSG(SYS_tuxcall)
		MSG(SYS_security)
		MSG(SYS_gettid)
		MSG(SYS_readahead)
		MSG(SYS_setxattr)
		MSG(SYS_lsetxattr)
		MSG(SYS_fsetxattr)
		MSG(SYS_getxattr)
		MSG(SYS_lgetxattr)
		MSG(SYS_fgetxattr)
		MSG(SYS_listxattr)
		MSG(SYS_llistxattr)
		MSG(SYS_flistxattr)
		MSG(SYS_removexattr)
		MSG(SYS_lremovexattr)
		MSG(SYS_fremovexattr)
		MSG(SYS_tkill)
		MSG(SYS_time)
		MSG(SYS_futex)
		MSG(SYS_sched_setaffinity)
		MSG(SYS_sched_getaffinity)
		MSG(SYS_set_thread_area)
		MSG(SYS_io_setup)
		MSG(SYS_io_destroy)
		MSG(SYS_io_getevents)
		MSG(SYS_io_submit)
		MSG(SYS_io_cancel)
		MSG(SYS_get_thread_area)
		MSG(SYS_lookup_dcookie)
		MSG(SYS_epoll_create)
		MSG(SYS_epoll_ctl_old)
		MSG(SYS_epoll_wait_old)
		MSG(SYS_remap_file_pages)
		MSG(SYS_getdents64)
		MSG(SYS_set_tid_address)
		MSG(SYS_restart_syscall)
		MSG(SYS_semtimedop)
		MSG(SYS_fadvise64)
		MSG(SYS_timer_create)
		MSG(SYS_timer_settime)
		MSG(SYS_timer_gettime)
		MSG(SYS_timer_getoverrun)
		MSG(SYS_timer_delete)
		MSG(SYS_clock_settime)
		MSG(SYS_clock_gettime)
		MSG(SYS_clock_getres)
		MSG(SYS_clock_nanosleep)
		MSG(SYS_exit_group)
		MSG(SYS_epoll_wait)
		MSG(SYS_epoll_ctl)
		MSG(SYS_tgkill)
		MSG(SYS_utimes)
		MSG(SYS_vserver)
		MSG(SYS_mbind)
		MSG(SYS_set_mempolicy)
		MSG(SYS_get_mempolicy)
		MSG(SYS_mq_open)
		MSG(SYS_mq_unlink)
		MSG(SYS_mq_timedsend)
		MSG(SYS_mq_timedreceive)
		MSG(SYS_mq_notify)
		MSG(SYS_mq_getsetattr)
		MSG(SYS_kexec_load)
		MSG(SYS_waitid)
		MSG(SYS_add_key)
		MSG(SYS_request_key)
		MSG(SYS_keyctl)
		MSG(SYS_ioprio_set)
		MSG(SYS_ioprio_get)
		MSG(SYS_inotify_init)
		MSG(SYS_inotify_add_watch)
		MSG(SYS_inotify_rm_watch)
		MSG(SYS_migrate_pages)
		MSG(SYS_openat)
		MSG(SYS_mkdirat)
		MSG(SYS_mknodat)
		MSG(SYS_fchownat)
		MSG(SYS_futimesat)
		MSG(SYS_newfstatat)
		MSG(SYS_unlinkat)
		MSG(SYS_renameat)
		MSG(SYS_linkat)
		MSG(SYS_symlinkat)
		MSG(SYS_readlinkat)
		MSG(SYS_fchmodat)
		MSG(SYS_faccessat)
		MSG(SYS_pselect6)
		MSG(SYS_ppoll)
		MSG(SYS_unshare)
		MSG(SYS_set_robust_list)
		MSG(SYS_get_robust_list)
		MSG(SYS_splice)
		MSG(SYS_tee)
		MSG(SYS_sync_file_range)
		MSG(SYS_vmsplice)
		MSG(SYS_move_pages)
		MSG(SYS_utimensat)
		MSG(SYS_epoll_pwait)
		MSG(SYS_signalfd)
		MSG(SYS_timerfd_create)
		MSG(SYS_eventfd)
		MSG(SYS_fallocate)
		MSG(SYS_timerfd_settime)
		MSG(SYS_timerfd_gettime)
		MSG(SYS_accept4)
		MSG(SYS_signalfd4)
		MSG(SYS_eventfd2)
		MSG(SYS_epoll_create1)
		MSG(SYS_dup3)
		MSG(SYS_pipe2)
		MSG(SYS_inotify_init1)
		MSG(SYS_preadv)
		MSG(SYS_pwritev)
		MSG(SYS_rt_tgsigqueueinfo)
		MSG(SYS_perf_event_open)
		MSG(SYS_recvmmsg)
		MSG(SYS_fanotify_init)
		MSG(SYS_fanotify_mark)
		MSG(SYS_prlimit64)
		MSG(SYS_name_to_handle_at)
		MSG(SYS_open_by_handle_at)
		MSG(SYS_clock_adjtime)
		MSG(SYS_syncfs)
		MSG(SYS_sendmmsg)
		MSG(SYS_setns)
		MSG(SYS_getcpu)
		MSG(SYS_process_vm_readv)
		MSG(SYS_process_vm_writev)
		MSG(SYS_kcmp)
		MSG(SYS_finit_module)
		MSG(SYS_sched_setattr)
		MSG(SYS_sched_getattr)
		MSG(SYS_renameat2)
		MSG(SYS_seccomp)
		MSG(SYS_getrandom)
		MSG(SYS_memfd_create)
		MSG(SYS_kexec_file_load)
		MSG(SYS_bpf)
		MSG(SYS_execveat)
		MSG(SYS_userfaultfd)
		MSG(SYS_membarrier)
		MSG(SYS_mlock2)
		MSG(SYS_copy_file_range)
		MSG(SYS_preadv2)
		MSG(SYS_pwritev2)
		MSG(SYS_pkey_mprotect)
		MSG(SYS_pkey_alloc)
		MSG(SYS_pkey_free)
		MSG(SYS_statx)
		MSG(SYS_io_pgetevents)
		MSG(SYS_rseq)
		MSG(SYS_pidfd_send_signal)
		MSG(SYS_io_uring_setup)
		MSG(SYS_io_uring_enter)
		MSG(SYS_io_uring_register)
		MSG(SYS_open_tree)
		MSG(SYS_move_mount)
		MSG(SYS_fsopen)
		MSG(SYS_fsconfig)
		MSG(SYS_fsmount)
		MSG(SYS_fspick)
		MSG(SYS_pidfd_open)
		MSG(SYS_clone3)
		MSG(SYS_close_range)
		MSG(SYS_openat2)
		MSG(SYS_pidfd_getfd)
		MSG(SYS_faccessat2)
		MSG(SYS_process_madvise)
		MSG(SYS_epoll_pwait2)
		MSG(SYS_mount_setattr)
		MSG(SYS_landlock_create_ruleset)
		MSG(SYS_landlock_add_rule)
		MSG(SYS_landlock_restrict_self)
		MSG(SYS_memfd_secret)
		MSG(SYS_process_mrelease)
		MSG(SYS_futex_waitv)
		MSG(SYS_set_mempolicy_home_node)
		MSG(SYS_cachestat)
		MSG(SYS_fchmodat2)
#undef MSG
return "UNDEFINED SYSCALL";
}
