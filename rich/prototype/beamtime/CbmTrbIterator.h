#ifndef CBMTRBITERATOR_H
#define CBMTRBITERATOR_H

#include "CbmRawEvent.h"
#include "CbmRawSubEvent.h"

class CbmTrbIterator
{
private:
	void* fData;
	size_t fSize;

	CbmRawEvent* fCurrentEvent;

	CbmRawSubEvent* fCurrentSubEvent;

public:

	CbmTrbIterator(void* data, size_t size):
		fData(data),
		fSize(size),
		fCurrentEvent(NULL),
		fCurrentSubEvent(NULL)
	{
		;
	}

	~CbmTrbIterator()
	{
		;
	}

	CbmRawEvent* NextEvent()
	{
		fCurrentSubEvent= NULL;
		if (fCurrentEvent == NULL){
			fCurrentEvent = new CbmRawEvent(fData);
			return fCurrentEvent;
		}
		void* nextEventPointer = (void*)((char*)(fCurrentEvent->fData) + fCurrentEvent->PaddedSize());
		if ( (char*)nextEventPointer - (char*)fData >= fSize ) return NULL;

		if (fCurrentEvent != NULL) delete fCurrentEvent;
		fCurrentEvent = new CbmRawEvent(nextEventPointer);
		return fCurrentEvent;
	}

	CbmRawEvent* CurrentEvent() const
	{
		return fCurrentEvent;
	}

	CbmRawSubEvent* NextSubEvent()
	{
		if (fCurrentEvent == NULL) return NULL;
		if (fCurrentEvent->Data() == fCurrentEvent->End()){
			return NULL;
		}

		if (fCurrentSubEvent == NULL ){

			fCurrentSubEvent = new CbmRawSubEvent(fCurrentEvent->Data());
			return fCurrentSubEvent;
		}

		uint32_t size = fCurrentSubEvent->Size();
		uint32_t alignment = fCurrentEvent->Alignment();
		size_t paddedSize = (size & (alignment - 1)) ? (size & ~(alignment - 1)) + alignment : size;
		void* nextSubEventPointer = (void *) ((char *) fCurrentSubEvent->fData + paddedSize);
		if (nextSubEventPointer < fCurrentEvent->End()) {
			if (fCurrentSubEvent != NULL) delete fCurrentSubEvent;
			fCurrentSubEvent = new CbmRawSubEvent(nextSubEventPointer);
			return fCurrentSubEvent;
		} else {
			return NULL;
		}
	}

	CbmRawSubEvent* currentSubEvent() const
	{
		return fCurrentSubEvent;
	}

 private:

	CbmTrbIterator(const CbmTrbIterator&);
	CbmTrbIterator operator=(const CbmTrbIterator&);
};

#endif // CBMTRBITERATOR_H
