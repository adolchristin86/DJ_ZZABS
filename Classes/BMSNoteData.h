#ifndef __BMS_KEY_DATA_H__
#define __BMS_KEY_DATA_H__

#include "ComDef.h"

class BMSNoteData {
public:
	double beat;		//비트
	int key;			//채널
	double value;		//노트
	long time;		//시간
	int attr;			//속성	0:연주 가능, 1: 연주 끝
	int index;			//번호
};

class BMSLNoteData : public BMSNoteData {
public:
	double beatStart;
	double beatEnd;

	bool operator< (const BMSLNoteData &o) {
		return (this->beatStart)<(o.beatStart);
	}
};

class BMSHeaderData {
public:
	std::string genre;
	std::string title;
	std::string artist;

	float bpm;

	int playlevel;
	int rank;

	std::string stagefile;
};
#endif // __BMS_KEY_DATA_H__
