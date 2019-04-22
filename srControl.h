#pragma once
#include <cstdint>

namespace srgui {

class srControl;

class srContainer
{
public:
	virtual srControl* getChild(uint32_t index) =0;
	virtual bool point_in_child(uint32_t index, const srPoint& p) =0;
	virtual void addChild(srControl*) =0;
	virtual uint32_t getNumChildren() =0;
};

class srIEvent
{
public:
	virtual void raiseClickEvent() {};

};

class srControl
{
public:
	virtual uint64_t getFlags() { return flags; }
	virtual void setFlags(uint32_t) =0;
	virtual void setArea(const srRect&) =0;
	virtual void getArea(srRect&) =0;
	virtual void draw(const srDrawInfo&) =0;
	virtual srControlType type() =0;

	virtual srControl* getParent() { return parent; }
	virtual void setParent(srControl* c) { parent = c; }
	
	friend void SendEvent(srEventType event, int data0, int data1, int data2, int data3);
protected:
	srRect area;
	srControl* parent;
	uint64_t flags;
};


} //end of srgui namespace


