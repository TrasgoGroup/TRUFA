
#ifndef THLDEVENT
#define THLDEVENT

/////////////////////////////////////////////////////////////////////
//   HldEvent
//
//   HldEvent is based on HldFilEvt, HldBase and HldEvt classes from Hydra.
/////////////////////////////////////////////////////////////////////

#include "thldsubevent.h"
#include <TObject.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h> // from times
#include <stdlib.h>
#include <sstream>
#include "TClonesArray.h"
#include "thit.h"
#include "tevent.h"

using namespace std;

typedef UInt_t UInt4;
typedef UChar_t UInt1;



class HldEvent :public TObject
{
private:
    Int_t totalNHits;
    std::istream* file;
    struct stat status; //! status struct of hld file
    Bool_t isWritable; // flag used for HLD output

    UInt_t subEvtId;                      // subevent id
    UInt_t uStartPosition;                // position at which to start decoding.
    UInt_t uTrbNetAdress;                 // TrbNetAdress
    UInt_t uSubBlockSize;                 // BlockSize of SubSubEvent (one TRB)
    UInt_t nCountWords;                   // at which data to start decoding

    Int_t trbDataVer;                     // data structure version:
    // info: http://hades-wiki.gsi.de/cgi-bin/view/DaqSlowControl/RpcDataStructure
    Int_t debugFlag;                      //! allows to print subevent information to the STDOUT

    Bool_t quietMode;                     //! do not print errors!
    Bool_t reportCritical;                //! report critical errors!
    Bool_t correctINL;                    //! if > 0 performs the INL correction
    Bool_t correctINLboard;               //! if > 0 performs the INL correction for this TRB
    Bool_t highResModeOn;                 //! is set the data are collected in High Res. Mode - 25ps binning
    Bool_t trbDataPairFlag;               //! data in pair mode (leading and width in one word)

    Float_t trbLeadingTime[128][10];
    Float_t trbTrailingTime[128][10];
    Float_t trbADC[128][10];

    UInt_t trbExtensionSize;
    Int_t trbLeadingMult[128];
    Int_t trbTrailingMult[128];
    Int_t trbTrailingTotalMult[128];      // FIXME: Pablos private version; total multiplicity for trailings
    Int_t trbDataExtension[128];



public:

    HldEvent(): pData(0) {                    // No input file
	file = 0;
	isWritable=kFALSE;
	//fpgaAddr=0;
	setSubEvtId();
	init();
    }
    UInt_t HexStrToInt(const char* str) {
	UInt_t t;
	std::stringstream s;
	s << std::hex << str;
	s >> t;
	return t;
    }
    HldEvent(const char* name,Int_t subId, const char* fpgaAddr): pData(0) {    // read from file
	file = 0;	//wk added 25.05.07
	//fpgasNum = 0;

	// gk retreive fpga addresses
	char temp[4];
	UInt_t ctr = 0;
        /*
	while (true) {

	    if (fpgaAddr[ctr] == NULL) {
		fpgasAddr[fpgasNum] = HexStrToInt(temp);
		fpgasNum++;
		break;
	    }
	    else if (fpgaAddr[ctr] == '|') {
		fpgasAddr[fpgasNum] = HexStrToInt(temp);
		ctr++;
		fpgasNum++;
	    }
	    else {
		temp[(ctr - fpgasNum) % 4] = fpgaAddr[ctr];
		ctr++;
	    }
	}
        */
	//if(!quietMode) printf("FPGA code: %s amount: %d\n", fpgaAddr, fpgasNum);

	//minWindow = -100000;
	//maxWindow = -100000;

	setFile(name);
	setSubEvtId(subId);
	init();
    }

    // gk
    HldEvent(const char* name,Int_t subId, const char* fpgaAddr, Int_t min, Int_t max): pData(0) {    // read from file
	file = 0;	//wk added 25.05.07
	//fpgasNum = 0;

	// gk retreive fpga addresses
	char temp[4];
	UInt_t ctr = 0;
        /*
	while (true) {

	    if (fpgaAddr[ctr] == NULL) {
		fpgasAddr[fpgasNum] = HexStrToInt(temp);
		fpgasNum++;
		break;
	    }
	    else if (fpgaAddr[ctr] == '|') {
		fpgasAddr[fpgasNum] = HexStrToInt(temp);
		ctr++;
		fpgasNum++;
	    }
	    else {
		temp[(ctr - fpgasNum) % 4] = fpgaAddr[ctr];
		ctr++;
	    }
	}
        */
	//if(!quietMode) printf("FPGA code: %s amount: %d\n", fpgaAddr, fpgasNum);

	//minWindow = min;
	//maxWindow = max;

	setFile(name);
	setSubEvtId(subId);
	init();
    }



    ~HldEvent() {
	if (file) delete file;
	delete[] pData;
    }

    size_t getHdrSize() const { return sizeof(EvtHdr); }
    UInt4 getSize() const { return ((EvtHdr*)pHdr)->size; }
    UInt4 getDecoding() const { return ((EvtHdr*)pHdr)->decoding; }
    UInt4 getId() const { return ((EvtHdr*)pHdr)->id; }
    UInt4 getSeqNr() const { return ((EvtHdr*)pHdr)->seqNr; }
    UInt4 getDate() const { return ((EvtHdr*)pHdr)->date; }
    UInt4 getTime() const { return ((EvtHdr*)pHdr)->time; }
    Int_t getYear() const { return byte(2, ((EvtHdr*)pHdr)->date); }
    Int_t getMonth() const { return byte(3, ((EvtHdr*)pHdr)->date) + 1; }
    Int_t getDay() const { return byte(4, ((EvtHdr*)pHdr)->date); }
    Int_t getHour() const { return byte(2, ((EvtHdr*)pHdr)->time); }
    Int_t getMinute() const { return byte(3, ((EvtHdr*)pHdr)->time); }
    Int_t getSecond() const { return byte(4, ((EvtHdr*)pHdr)->time); }
    Int_t getRunNr() const { return ((EvtHdr*)pHdr)->runNr; }
    UInt4 getPad() const { return ((EvtHdr*)pHdr)->pad; }
    // Get data body
    UInt4* getHeader() const { return pHdr; }
    UInt4* getData() const { return pData; }
    size_t getDataSize() const { return getSize() - getHdrSize(); }
    size_t getPaddedSize() const { return align8(getSize()); }
    UInt4* getEnd() const { return pData + getDataSize()/4; }
    UInt4* getPaddedEnd() const { return pData + getDataPaddedSize()/4; }

    void setWritable(Bool_t f=kTRUE) { isWritable=f; }
    Bool_t setFile(const Text_t *name);
    Bool_t readSubEvt(size_t i);
    Bool_t read();
    Bool_t execute();
    Bool_t swap();
    //from HldEvt
    void setSubEvtId(Int_t f=0) { subEvtId=f; }
    Int_t appendSubEvtIdx();

    Int_t correctRefTimeCh31(void);
    Hit* addHit(TClonesArray* Hits);

public:

    void init() {
	pHdr = (UInt4*) &hdr;
	pData = 0;
	lastSubEvtIdx = 0;
	appendSubEvtIdx();
	clearAll();
    }

protected:
    // Event header
    struct EvtHdr {
	UInt4 size;
	UInt4 decoding;
	UInt4 id;
	UInt4 seqNr;
	UInt4 date;
	UInt4 time;
	UInt4 runNr;
	UInt4 pad;
    } hdr;

    UInt4* pHdr;
    UInt4* pData;

    size_t lastSubEvtIdx;
    static const size_t  maxSubEvtIdx = 100;
    struct SubEvtTable {
	UInt4 id;
	HldSubEvent** p;
    };

    SubEvtTable subEvtTable[maxSubEvtIdx];

    // subevent list (preliminary)
    static const size_t maxSubEvts = 100;
    HldSubEvent subEvt[maxSubEvts];

    size_t getHdrLen() const { return getHdrSize()/4; }
    size_t getDataPaddedSize() const { return align8(getDataSize()); }
    Bool_t isSwapped() const { return byte(1, getDecoding()); }
    void swapHdr() { swap4(pHdr, getHdrLen()); }
    int byte(int b, UInt4 i) const { return (i >> 32 - 8*b & 0xff); }
    size_t align8(const size_t i) const { return 8*size_t( (i - 1)/8 + 1 ); }
    void swap4(UInt_t* p, const size_t len) const;
    void swap2(UShort_t* p, const size_t len) const;
    size_t getDataLen() const { return (getDataSize()+3)/4; }

    //wk added 31.01.07
    static const Int_t kMaxMult=10;
    static const Int_t kMaxChannelNr=10 * 128;

public:
    Int_t getkMaxMult() const { return kMaxMult; }
    Int_t getkMaxChannelNr() const { return kMaxChannelNr; }
    //wk end

    //wk from testunpacker
    //wk from HldUnpack
public:
    typedef HldSubEvent ** HPP;
    HPP const getpSubEvt(void);
    //wk void setCategory(HCategory *aCat);
protected:
    HldSubEvent* pSubEvt; //! pointer to subevent where data are read from
    //end from HldUnpack
public:
    //UInt_t subEvtId;

    Int_t LeadingTime[kMaxChannelNr][kMaxMult];
    Int_t TrailingTime[kMaxChannelNr][kMaxMult];
    Int_t WidthTime[kMaxChannelNr][kMaxMult];
    //Int_t trbADC[kMaxChannelNr][kMaxMult];
    Int_t LeadingMult[kMaxChannelNr];
    Int_t WidthMult[kMaxChannelNr];
    Int_t TrailingMult[kMaxChannelNr];
    Int_t errors_per_event;          // number of errors per event
    Int_t SpikesCtr[kMaxChannelNr];

    //Int_t debugFlag;  // allows to print subevent information
    // to the STDOUT
    Int_t debugFlag1; // used for special purpuses
    // will be removed later
    //Bool_t quietMode; //! do not print errors!
    Bool_t fullSetup;
    Bool_t VHR;

public:
    //wk added 04.02.07
    Int_t getLeadingTime(Int_t channel,Int_t mult) const;
    Int_t getTrailingTime(Int_t channel,Int_t mult) const;
    Int_t getWidthTime(Int_t channel,Int_t mult) const;
    Int_t getLeadingMult(Int_t channel) const;
    Int_t getTrailingMult(Int_t channel) const;
    Int_t getSpikesCtr(Int_t channel) const;


    Int_t getSubEvtId(void) const { return subEvtId; }
    Int_t decode();
    Int_t decode(Int_t num);
    Float_t doINLCorrection(Int_t nTrbCh, Int_t nRawTime);
    Bool_t fill_trail(Int_t ch, Float_t time);
    Bool_t fill_lead(Int_t ch, Float_t time);
    Bool_t fill_pair(Int_t ch,Float_t time,Float_t length);
    void printTdcError(UInt_t e, UInt_t trbId);




    void clearAll(void);
    //void setQuietMode(void){quietMode=kTRUE;}kFALSE
    void setQuietMode(bool t) { quietMode=t; }
    void setFullSetup(bool t) { fullSetup = t; }
    void setVHR(bool t) { VHR = t; }
    void setDebugFlag(Int_t db){ debugFlag = db;};
    void setDebugFlag1(Int_t db){ debugFlag1 = db;};

    void incErrorNr() {errors_per_event++;}
    void PrintTdcError(UInt_t e);
    Bool_t fill_trail(Int_t ch,Int_t d);
    Bool_t fill_lead(Int_t ch,Int_t d);
    Bool_t fill_width(Int_t ch,Int_t d);
    // decode();
    void  fillTrbRaw(Int_t num);
    //end from testunpacker

public:
    ClassDef(HldEvent,0)
};

#endif /* !THLDEVENT */
