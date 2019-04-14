#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <sys/times.h>

#define HZ	100

void cpuio_bound(int last, int cpu_time, int io_time);
/*
1.所有子进程并行运行,每个子进程的实际运行时间一般不超过30s
2.父进程向标准输出打印所有子进程的id,并在所有子进程退出后才退出
*/
int main(int argc, char * argv[])
{
    int i = 0;
    int cnt = 0;
    int arr[10];
    printf("i son/par ppid pid fpid\n");
    for(i = 0; i < 3; i++)
    {
        pid_t fpid = fork();
        if(!fpid){
            printf("%d son     %4d %4d %4d \n", i, getppid(), getpid(), fpid);
            cpuio_bound(9, i*3, (3-i)*3);

        }
        else{
            arr[cnt++] = fpid;
            printf("%d par     %4d %4d %4d \n", i, getppid(), getpid(), fpid);
        }
    }
    for(i = 0; i < cnt; i++)
        waitpid(arr[i],NULL,0);
	return 0;
}

/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start_time, current_time;
	clock_t utime, stime;
	int sleep_time;

	while (last > 0)
	{
		/* CPU Burst */
		times(&start_time);
		/* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
		 * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
		 * 加上很合理。*/
		do
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		} while ( ( (utime + stime) / HZ )  < cpu_time );
		last -= cpu_time;

		if (last <= 0 )
			break;

		/* IO Burst */
		/* 用sleep(1)模拟1秒钟的I/O操作 */
		sleep_time=0;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
	}
}
