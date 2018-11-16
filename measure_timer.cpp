#include <poll.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <iostream>
#include <time.h>


#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#endif

const int N=1000000000;

void measure_clock_gettime()
{
	struct timespec tp1, tp2, tmp;
	clock_gettime(CLOCK_MONOTONIC, &tp1);
	time_t sec1 = tp1.tv_sec;
	long nanosec1 = tp1.tv_nsec;
	
	for(int i=0;i<N;++i)
		clock_gettime(CLOCK_MONOTONIC, &tmp);

	clock_gettime(CLOCK_MONOTONIC, &tp2);
	time_t sec2 = tp2.tv_sec;
	long nanosec2 = tp2.tv_nsec;
	std::cout << "sec1=" << sec1 << ", sec2=" << sec2

			  << ", nanosec1=" << nanosec1 << ", nanosec2=" << nanosec2 << std::endl;
	std::cout << "delta_sec=" << sec2-sec1 << ", delta_nanosec=" << nanosec2-nanosec1 << std::endl;
		
	std::cout << "prevent optimization " << tmp.tv_sec << std::endl;
	double cost = (double)((sec2-sec1)*1000000000 + (nanosec2-nanosec1))/(double)N;
	std::cout << "clock_gettime nanosec per call = " << cost << std::endl;
}

void measure_rdtsc()
{
	struct timespec tp1, tp2, tmp;
	clock_gettime(CLOCK_MONOTONIC, &tp1);
	time_t sec1 = tp1.tv_sec;
	long nanosec1 = tp1.tv_nsec;
	
	unsigned long long r1=rdtsc(), r2;
	for(int i=0;i<N;++i)
		r2=rdtsc();

	clock_gettime(CLOCK_MONOTONIC, &tp2);
	time_t sec2 = tp2.tv_sec;
	long nanosec2 = tp2.tv_nsec;
	std::cout << "sec1=" << sec1 << ", sec2=" << sec2

			  << ", nanosec1=" << nanosec1 << ", nanosec2=" << nanosec2 << std::endl;
	std::cout << "delta_sec=" << sec2-sec1 << ", delta_nanosec=" << nanosec2-nanosec1 << std::endl;
		
	std::cout << "prevent optimization sec passed " << (double)(r2-r1)/2670000000 << std::endl;
	double cost = (double)((sec2-sec1)*1000000000 + (nanosec2-nanosec1))/(double)N;
	std::cout << "rdtsc nanosec per call = " << cost << std::endl;
}

int main(int argc, char *argv[])
{
	measure_clock_gettime();
	measure_rdtsc();
	unsigned long long r1 = rdtsc();
	std::cout << "since reboot " << (double)r1/(double)2670000000 << std::endl;
}
