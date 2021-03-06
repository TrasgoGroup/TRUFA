#ifndef HIT
#define HIT


#include <TObject.h>
#include <TBuffer.h>


class Hit:public TObject
{
public:
    Hit()
    {
	clear();
    }
    //wk temporary
    Hit(Int_t ch,Int_t tdc,Int_t trbnum)
    {
	setChannel(ch);
	setTDC(tdc);
        setTrbNum(trbnum);
    }
protected:

    Int_t channel;	// channel number
    Int_t TDC;	        // TDC number from 0-3
    Int_t nHits;        // number of hits in channel in one event (up to 4)
    Int_t trbNum;


    //  Int_t time1;    // tdc time of 1st hit
    //  Int_t width1;   // width of 1st hit
    //  Int_t time2;    // tdc time of 2nd hit
    //  Int_t width2;   // width of 2nd hit
    //  Int_t time3;    // tdc time of 3rd hit
    //  Int_t width3;   // width of 3rd hit
    //  Int_t time4;    // tdc time of 4th hit
    //  Int_t width4;   // width of 4th hit

    //wk added 17.01.07
    static const Int_t kMaxMult;
    Int_t width;	// leading time - trailing time

    //wk added 14.03.07
    Int_t leadTime1;
    Int_t trailTime1;
    Int_t leadTime2;
    Int_t trailTime2;
    Int_t leadTime3;
    Int_t trailTime3;
    Int_t leadTime4;
    Int_t trailTime4;
    Int_t spikesCtr;

public:

    void setChannel(Int_t c) {channel=c;}
    Bool_t setTDC(Int_t t){TDC=t;}
    void setNHits(Int_t h) { nHits=h; }
    void setTrbNum(Int_t num) {trbNum = num;}
    Bool_t fillTimeAndWidthAndTrb(Int_t time, Int_t widthq, Int_t num) {
	width = widthq;
        leadTime1 = time;
        trbNum = num;
    }
    //  void setTime1(Int_t t) { time1=t; }
    //  void setWidth1(Int_t w){ width1=w; }
    //  void setTime2(Int_t t) { time2=t; }
    //  void setWidth2(Int_t w) { width2=w;}
    //  void setTime3(Int_t t)  { time3=t;}
    //  void setWidth3(Int_t w) { width3=w; }
    //  void setTime4(Int_t t)  { time4=t; }
    //  void setWidth4(Int_t w) { width4=w; }

    //wk added 17.01.07
    //gk 03.12.10
    void setWidth(Int_t lead,Int_t trail) { width=trail-lead; } //lead-trail; }

    //wk added 14.03.07
    void setLeadTime1(Int_t t) { leadTime1=t; }
    void setTrailTime1(Int_t t) { trailTime1=t; }
    void setLeadTime2(Int_t t) { leadTime2=t; }
    void setTrailTime2(Int_t t) { trailTime2=t; }
    void setLeadTime3(Int_t t) { leadTime3=t; }
    void setTrailTime3(Int_t t) { trailTime3=t; }

    void setLeadTime4(Int_t t) { leadTime4=t; }
    void setTrailTime4(Int_t t) { trailTime4=t; }

    void setSpikesCtr(Int_t t) { spikesCtr=t; }

    Int_t getChannel() { return channel;}
    Int_t getTDC() { return TDC;}
    Int_t getNHits() { return nHits; }
    Int_t getTRBNum() {return trbNum;}

    //wk added 17.01.07
    Int_t getWidth() { return width; }

    //wk added 14.03.07
    Int_t getLeadTime1() { return leadTime1; }
    Int_t getTrailTime1() { return trailTime1; }
    Int_t getLeadTime2() { return leadTime2; }
    Int_t getTrailTime2() { return trailTime2; }
    Int_t getLeadTime3() { return leadTime3; }
    Int_t getTrailTime3() { return trailTime3; }
    Int_t getLeadTime4() { return leadTime4; }
    Int_t getTrailTime4() { return trailTime4; }
    Int_t getSpikesCtr() { return spikesCtr; }

    //wk from HHodoRaw
    Int_t getTime(const Int_t n) const;
    Int_t getWidth(const Int_t n) const;
    void getTimeAndWidth(const Int_t,Int_t&,Int_t&);
    //wk  void setMult(Int_t mu) { 1; } //nHits=mu;
    Bool_t fillTimeAndWidth(const Int_t,const Int_t);
    Bool_t fill_lead(const Int_t time);
    Bool_t fill_trail(const Int_t time);
    //wk end from HHodoRaw


    void clear(void);

public:
    ClassDef(Hit,1);
};

#endif  /* !HIT */
