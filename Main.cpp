#include "TSArray.h"
#include <iostream>
#include <string>

using namespace std;

static void TestThread1(TSArray<int> *ap);
static void TestThread2(TSArray<int> *ap);

static boost::barrier bar(2);

//Example/minimal test harness
int main(int argc, char** argv)
{
	TSArray<int> s(1000); //A 1k element array

	//Can be treated like a normal array
	s[0] = 5;
	s[1] = s[0];

	//Provides granular thread safety for individual elements	
	boost::thread t1(boost::bind(&TestThread1, &s));
	boost::thread t2(boost::bind(&TestThread2, &s));

	t1.join();
	t2.join();

	cout << "Array dump (will vary significantly based on scheduling, but no race condition weirdness):" << endl;

	for(int i=0;i<1000;i++)
		cout << s[i] << " ";

	getchar();
}

static void TestThread1(TSArray<int> *ap)
{
	cout << "Thread 1 locking element 0." << endl;
	ap->Lock(0);
	boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
	(*ap)[0] = 10;
	cout << "Thread 1 releasing element 0." << endl;
	ap->Unlock(0);

	bar.wait(); //resynchronise

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	cout << "Thread 1 attempting to lock and set element 1." << endl;
	ap->Lock(1);
	(*ap)[1] = (*ap)[0] + 2;
	ap->Unlock(1);

	cout << "Thread 1 attempting to lock and set element 2." << endl;
	ap->Lock(2);
	(*ap)[2] = (*ap)[1] + 2;
	ap->Unlock(2);

	cout << "Thread 1 attempting to lock and set element 3." << endl;
	ap->Lock(3);
	(*ap)[3] = (*ap)[2] + 2;
	ap->Unlock(3);

	cout << "Thread 1 attempting to lock and set element 4." << endl;
	ap->Lock(4);
	(*ap)[4] = (*ap)[3] + 2;
	ap->Unlock(4);

	cout << "Thread 1 attempting to lock and set element 5." << endl;
	ap->Lock(5);
	(*ap)[5] = (*ap)[4] + 2;
	ap->Unlock(5);

	bar.wait(); //resynchronise

	//A more "real world" example in which two threads are inevitably going to interact

	for(unsigned i=6;i<1000;i++)
	{
		ap->Lock(i);
		(*ap)[i] = (*ap)[i-1] + 1;
		ap->Unlock(i);
	}
}

static void TestThread2(TSArray<int> *ap)
{
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	cout << "Thread 2 attempting to lock element 0." << endl;
	ap->Lock(0); //Attempt to lock element 0; will block for 4 seconds.
	cout << "Thread 2 locked element 0." << endl;
	(*ap)[0] = 12;

	bar.wait(); //resynchronise

	cout << "Thread 2 locking element 3." << endl;
	ap->Lock(3);
	boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
	cout << "Thread 2 releasing element 3." << endl;
	ap->Unlock(3);

	bar.wait(); //resynchronise

	//A more "real world" example in which two threads are inevitably going to interact

	for(unsigned i=6;i<1000;i++)
	{
		ap->Lock(i);
		(*ap)[i] = (*ap)[i-1] - 1;
		ap->Unlock(i);
	}
}