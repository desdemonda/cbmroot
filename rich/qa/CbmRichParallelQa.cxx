/** CbmRichParallelQa.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/

#include "CbmRichParallelQa.h"
#include "CbmRichHit.h"
#include "FairRootManager.h"
#include "tbb/tick_count.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/task.h"
#include "tbb/parallel_invoke.h"
#include "tbb/task_scheduler_observer.h"
#include "tbb/spin_mutex.h"
#include "tbb/parallel_for.h"

#include <map>
#include <iostream>
#include <fstream>

using namespace std;
using namespace tbb;

int threads_counter = -1;
map<int, long> threadToCpuMap; // let get cpuId by threadId
map<int, int> threadNumberToCpuMap; // let get cpuId by threadNumber (see threads_counter)
spin_mutex mutex;

class TMyObserver: public task_scheduler_observer {
public:
	void FInit(); // set cpu - thread correspondence
protected:
	void on_scheduler_entry(bool Is_worker); // run at begin of each thread execution
	void on_scheduler_exit(bool Is_worker); // run at end of each thread execution
};

// set cpu - thread correspondence
void TMyObserver::FInit() {
	for (int i = 0; i < 8; i++) {
		//threadNumberToCpuMap[2 * i + 0] = i;
		//threadNumberToCpuMap[2 * i + 1] = i + 8;
		threadNumberToCpuMap[i] = i;
		threadNumberToCpuMap[8 + i] = i + 8;

	};
	observe(true);
}

/// redefine function, which will be run at begining of execution of each thread
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
void TMyObserver::on_scheduler_entry(bool Is_worker) {
	//cout << "-I-Scheduler entry" <<endl;
	pthread_t I = pthread_self();
	spin_mutex::scoped_lock lock;
	lock.acquire(mutex);
	++threads_counter;
	int cpuId = threadNumberToCpuMap[threads_counter % 16];

	//cout << "ThrId=" << I << " thread have been created " << threads_counter << "-th.";
	//cout << " And was run on cpu " << cpuId << endl;

	lock.release();
	threadToCpuMap[I] = cpuId;

	int s, j;
	cpu_set_t cpuset;
	pthread_t thread = I;
	CPU_ZERO(&cpuset);
	CPU_SET(cpuId, &cpuset);
	// cout << "before" << endl; //FIXME: segmentation fault somethere.
	s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (s != 0) {
		cout << " pthread_setaffinity_np " << endl;
//		handle_error_en(s, "pthread_setaffinity_np");
	}
};

// run at end of each thread execution
void TMyObserver::on_scheduler_exit(bool Is_worker) //FIXME: don't run
{
	pthread_t I = pthread_self();
	cout << "Thread with number " << I << " was ended " << --threads_counter;
};


class FinderTaskQa : public task {
	CbmL1RichENNRingFinder* fHT;
	//CbmRichRingFinderHough* fHT;
    std::vector<std::vector<CbmRichHoughHit> > fData;
	// std::vector<CbmRichHoughHit> fData;
public:
	FinderTaskQa(CbmL1RichENNRingFinder* HTImpl,
			  const std::vector<std::vector<CbmRichHoughHit> > &data) {
		fHT = HTImpl;
		fData = data;
		//fData.assign(data[0].begin(), data[0].end());
	}
	task* execute(){
		//for (int j = 0; j < 10; j ++){
			for (int i = 0; i < fData.size(); i ++){
				//cout<< "rec event "<< i << endl;
				fHT->DoFind(fData[i]);
			}
		//}//
		return NULL;
	}
};


CbmRichParallelQa::CbmRichParallelQa()
{
	for (int i = 0; i < kMAX_NOF_THREADS;i++){
		//fHT[i] = new CbmRichRingFinderHough(0, "compact");
		fHT[i] = new CbmL1RichENNRingFinder(0);
	}

	fEventNumber = 0;
	fExecTime = 0.;
}

CbmRichParallelQa::~CbmRichParallelQa()
{
	delete fHT;
}

InitStatus CbmRichParallelQa::Init()
{
    cout << "InitStatus CbmRichParallelQa::Init()" << endl;

	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmRichParallelQa::Init","RootManager not instantised!");}

	fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
	if (NULL == fRichHits) { Fatal("CbmRichParallelQa::Init","No RichHit array!");}

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if (NULL == fRichRings) { Fatal("CbmRichParallelQa::Init","No RichRing array!");}

	for (int i = 0; i<kMAX_NOF_THREADS;i++){
		fHT[i]->Init();
	}

//	tbb::task_scheduler_init init();
//	TMyObserver obs;
//	obs.FInit();    // set cpu-threads correspondence

    return kSUCCESS;
}

void CbmRichParallelQa::Exec(
      Option_t* option)
{
	fEventNumber++;
	cout << "-I- Read event " << fEventNumber<<endl;
	std::vector<CbmRichHoughHit> data;

	const Int_t nhits = fRichHits->GetEntriesFast();
	if (!nhits) {
		cout << "-E- CbmRichRingFinderHough::DoFind:No hits in this event."	<< endl;
		return;
	}
	data.reserve(nhits);

	for(Int_t iHit = 0; iHit < nhits; iHit++) {
		CbmRichHit * hit = (CbmRichHit*) fRichHits->At(iHit);
		if (hit) {
			CbmRichHoughHit tempPoint;
			tempPoint.fHit.fX = hit->GetX();
			tempPoint.fHit.fY = hit->GetY();
			tempPoint.fX2plusY2 = hit->GetX() * hit->GetX() + hit->GetY() * hit->GetY();
			tempPoint.fId = iHit;
			tempPoint.fIsUsed = false;
			data.push_back(tempPoint);
		}
	}

	fData.push_back(data);

	if (fEventNumber == fNofEvents){
		cout << "-I- NofTasks = " << fNofTasks << endl;
		TMyObserver obs;
		obs.FInit(); // set cpu-threads correspondence
		obs.observe(true);
		tbb::task_scheduler_init init(fNofTasks);
		DoTestWithTask();
	}

}

void CbmRichParallelQa::DoTestWithTask()
{
	tbb::tick_count t0 = tbb::tick_count::now();
	task* root_task = new (task::allocate_root()) empty_task;
	root_task->set_ref_count(fNofTasks+1);
	task_list list;

	for (int iT = 0; iT < fNofTasks; iT++){
		list.push_back(*new(root_task->allocate_child()) FinderTaskQa (fHT[iT], fData));
	}

	root_task->spawn_and_wait_for_all(list);
	tbb::tick_count t1 = tbb::tick_count::now();

	root_task->destroy(*root_task);
	fExecTime += (t1-t0).seconds();
	cout <<  1000.*fExecTime << " ms for " << fData.size()<< " events" << endl;
	cout <<  1000.*fExecTime/ (fData.size())<< " ms per event "<< endl;
	cout <<  fNofTasks*fData.size()/fExecTime << " events per sec"<< endl;

	std::ofstream fout;
	fout.open("parallel.txt",std::ios_base::app);
	fout << (int)(fNofTasks*fData.size()/fExecTime) << ",";
}
