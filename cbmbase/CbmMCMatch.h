/*
 * CbmMCMatch.h
 *
 *  Created on: Nov 23, 2009
 *      Author: stockman
 */

#ifndef CBMMCMATCH_H_
#define CBMMCMATCH_H_


#include "CbmDetectorList.h"
#include "CbmMCStage.h"
#include "CbmMCResult.h"
#include "CbmMCEntry.h"

#include "FairLink.h"
#include "FairMultiLinkedData.h"

#include "TNamed.h"

#include <map>

class FairMultiLinkedData;

typedef std::map<Cbm::DataType, CbmMCStage*>::iterator TListIterator;
typedef std::map<Cbm::DataType, CbmMCStage*>::const_iterator TListIteratorConst;

class CbmMCMatch: public TNamed {
public:
	CbmMCMatch();
	CbmMCMatch(const char* name, const char* title)
	  :TNamed(name, title), fUltimateStage(Cbm::kMCTrack),
	  fList(), fFinalStageML() {};
	virtual ~CbmMCMatch();

	void AddElement(Cbm::DataType type, int index, FairLink link);
	void AddElement(Cbm::DataType sourceType, int index, Cbm::DataType targetType, int link);
	void SetElements(Cbm::DataType sourceType, int index, FairMultiLinkedData* links);
	void InitStage(Cbm::DataType type, std::string fileName, std::string branchName);
	void RemoveStage(Cbm::DataType type);
	void LoadInMCLists(TClonesArray* myLinkArray);
	void ClearMCList();

	void SetCommonWeightStages(Float_t weight);

	CbmMCEntry GetEntry(Cbm::DataType type, int index);
	CbmMCEntry GetEntry(FairLink link);

	CbmMCResult GetMCInfo(Cbm::DataType start, Cbm::DataType stop);
	CbmMCEntry  GetMCInfoSingle(FairLink aLink, Cbm::DataType stop);

	int GetNMCStages() const {return fList.size();}

	CbmMCStage* GetMCStage(int index) const{
		TListIteratorConst iter = fList.begin();
		for (int i = 0; i < index; i++)
			iter++;
		return (iter->second);
	}

	CbmMCStage* GetMCStageType(Cbm::DataType type){
		return fList[type];
	}

	void CreateArtificialStage(Cbm::DataType stage, std::string fileName = "", std::string branchName = "");

	FairMultiLinkedData FindLinksToStage(Cbm::DataType stage);

	bool IsTypeInList(Cbm::DataType type);

	void print(std::ostream& out = std::cout){out << *this;}

	friend std::ostream& operator<< (std::ostream& out, const CbmMCMatch& match){
		for (int i = 0; i < match.GetNMCStages(); i++){
			if (match.GetMCStage(i)->GetLoaded() == kTRUE){
				match.GetMCStage(i)->print(out);
				out << std::endl;
			}
		}
		return out;
	}

private:
	Cbm::DataType fUltimateStage; ///< last stage in link chain. Here all recursive operations must stop.
	std::map<Cbm::DataType, CbmMCStage*> fList;
	FairMultiLinkedData fFinalStageML;

	void FindStagesPointingToLinks(FairMultiLinkedData links, Cbm::DataType stop);
	FairMultiLinkedData FindStagesPointingToLink(FairLink link);

	CbmMCResult GetMCInfoForward(Cbm::DataType start, Cbm::DataType stop);
	CbmMCResult GetMCInfoBackward(Cbm::DataType start, Cbm::DataType stop);
	CbmMCEntry GetMCInfoForwardSingle(FairLink link, Cbm::DataType stop);
	CbmMCEntry GetMCInfoBackwardSingle(FairLink link, Cbm::DataType stop, Double_t weight = 1.);

	void GetNextStage(FairMultiLinkedData& startEntry, Cbm::DataType stopStage);
	void AddToFinalStage(FairLink link, Float_t mult);
	void ClearFinalStage();
	ClassDef(CbmMCMatch, 1);
};

#endif /* PNDMCMATCH_H_ */
