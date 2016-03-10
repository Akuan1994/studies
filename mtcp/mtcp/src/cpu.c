#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <numa.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <assert.h>
#include "mtcp_api.h"

#define MAX_FILE_NAME 1024

/*----------------------------------------------------------------------------*/
// ��ȡ��ǰ��cpu����
int 
GetNumCPUs() 
{
	// ��ȡ��ǰ��cpu����
	return sysconf(_SC_NPROCESSORS_ONLN);
}
/*----------------------------------------------------------------------------*/
// ��ȡ��ǰ�̵߳�tid
pid_t 
Gettid()
{
	// ��ȡ��ǰ�̵߳�tid
	return syscall(__NR_gettid);
}
/*----------------------------------------------------------------------------*/
// �󶨵�ǰ�̵߳�ָ��id��cpu
int 
mtcp_core_affinitize(int cpu)
{
	cpu_set_t cpus;	//��һ���������飬һ����1024λ��ÿһλ�����Զ�Ӧһ��cpu����  
	struct bitmask *bmask;
	FILE *fp;
	char sysfname[MAX_FILE_NAME];
	int phy_id;
	size_t n;
	int ret;
	int unused;

	n = GetNumCPUs();

	if (cpu < 0 || cpu >= (int) n) {
		errno = -EINVAL;
		return -1;
	}

	// ����λ��0
	CPU_ZERO(&cpus);
	// ������Ҫ�󶨵�cpu id
	CPU_SET((unsigned)cpu, &cpus);

	// �󶨵�ǰ�߳�tid��ָ����cpu
	ret = sched_setaffinity(Gettid(), sizeof(cpus), &cpus);

	// ���ص�ǰϵͳ�е�numa node���������û�У�ֱ�ӷ���
	if (numa_max_node() == 0)
		return ret;

	bmask = numa_bitmask_alloc(n);
	assert(bmask);

	/* read physical id of the core from sys information */
	snprintf(sysfname, MAX_FILE_NAME - 1, 
			"/sys/devices/system/cpu/cpu%d/topology/physical_package_id", cpu);
	fp = fopen(sysfname, "r");
	if (!fp) {
		perror(sysfname);
		errno = EFAULT;
		return -1;
	}
	unused = fscanf(fp, "%d", &phy_id);

	// ����memory���׺���
	numa_bitmask_setbit(bmask, phy_id);
	numa_set_membind(bmask);
	numa_bitmask_free(bmask);

	fclose(fp);

	UNUSED(unused);
	return ret;
}
