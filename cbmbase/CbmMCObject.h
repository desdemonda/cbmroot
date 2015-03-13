/*
 * CbmMCStage.h
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#ifndef CBMMCOBJECT_H_
#define CBMMCOBJECT_H_

#include "CbmDetectorList.h"
#include "CbmMCEntry.h"

#include "TObject.h"

#include <vector>
#include <iostream>

class FairMultiLinkedData;

class CbmMCObject: public TObject {
 public:
  CbmMCObject();
 CbmMCObject(Cbm::DataType type):TObject(), fStage(), fStageId(type){};
 CbmMCObject(const CbmMCObject& obj)
   : TObject(), fStage(obj.GetEntryVector()), fStageId(obj.GetStageId())
    {};
 CbmMCObject(Cbm::DataType type, std::vector<CbmMCEntry> stage)
   : TObject(),
    fStage(stage),
    fStageId(type)
	{
	};
	virtual ~CbmMCObject();

	void SetStageId(Cbm::DataType type){fStageId = type;}

	void SetEntry(std::vector<Int_t> type, std::vector<Int_t> link, int index);
	void SetEntry(FairMultiLinkedData* data, int index);
	void SetEntry(CbmMCEntry link);

	void SetStage(std::vector<CbmMCEntry> stage) {fStage = stage;}
	void SetLink(FairLink link, int index);
	void AddLink(FairLink link, int index);

	CbmMCEntry GetEntry(int index) const {return fStage[index];}
	FairLink	GetSingleLink(int entryIndex, int linkIndex) const {
		return fStage.at(entryIndex).GetLink(linkIndex);
	}

	Cbm::DataType GetStageId(void) const {return fStageId;}

	CbmMCEntry GetMCLink(Int_t index){return fStage.at(index);}

	int GetNEntries() const {return fStage.size();}
	int GetNLinks(int entryIndex)const {return fStage.at(entryIndex).GetNLinks();}
	std::vector<CbmMCEntry> GetEntryVector() const{return fStage;}

//	FairMultiLinkedData PosInList(std::pair<int, int> link);
	FairMultiLinkedData PosInList(FairLink link);

	FairMultiLinkedData GetLinksWithType(Cbm::DataType type){
		FairMultiLinkedData result;
		for (int i = 0; i < GetNEntries(); i++){
			result.AddLinks(GetMCLink(i).GetLinksWithType(type), false);
		}
		return result;
	}

	virtual void ClearEntries(){fStage.clear();}

	virtual void print(std::ostream& out = std::cout){out << *this;}

	CbmMCObject& operator=(const CbmMCObject& obj){
	  TObject::operator=(obj);
	  fStageId = obj.GetStageId();
	  fStage = obj.GetEntryVector();
          return *this;
	}

	friend std::ostream& operator<< (std::ostream& out, const CbmMCObject& obj){
		std::vector<CbmMCEntry> stages = obj.GetEntryVector();
		for (int i = 0; i < stages.size(); i++){
			out << i << ": ";
			stages[i].print(out);
		}
		return out;
	}

private:
	void AdoptSize(int index);
	std::vector<CbmMCEntry> fStage;
	Cbm::DataType fStageId;



	ClassDef(CbmMCObject, 0);
};

#endif /* PNDMCOBJECT_H_ */
