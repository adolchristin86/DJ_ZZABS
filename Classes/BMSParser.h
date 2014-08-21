#ifndef __BMS_PARSER_H__
#define __BMS_PARSER_H__

#include "ComDef.h"
#include "Singleton.h"
#include "iostream"
#include "vector"
#include "list"
#include "BMSNoteData.h"

const int MAXVALUE = 1296;

//////////////////////////////////////////////
/// BMS 파싱에 대한 기본 정보
/// http://cosmic.mearie.org/2005/03/bmsguide/
//////////////////////////////////////////////
class CBMSParser : public CSingleton<CBMSParser>
{
	
public:

	enum EnumParserMode {
		EM_PARSER_MODE_NONE = -1,
		EM_PARSER_MODE_HEADER = 0,
		EM_PARSER_MODE_MAINDATA,
		EM_PARSER_MODE_MAX
	};

	CBMSParser();
	~CBMSParser();

	bool loadBMSFile(std::string strFileName, bool bHeaderOnly = false);
	bool getBMSHeader(BMSHeaderData& pHeaderData);	//헤더파일만 가져왔을 경우 헤더데이터를 넣어줌
	void setNoteTime();

	double getBPMArray(int nKeyValue);
	double getSTOPArray(int nKeyValue);

	double getBeatFromTime(long lMillisec);			//재생 시간으로부터 비트를 가져옴
	double getBPMFromBeat(double dBeat);			//비트로 부터 BPM을 가져옴

	BMSNoteData* getValidNoteData(int nKey);		//키 채널에 해당하는 유효한 노트 데이터 가져옴

	int getTotalMadiCount();

	inline int getTotalNote() { return m_nTotalNote; }
	
	inline double getLengthBeat(int nIdx) { return m_aLenthBeat[nIdx]; } //채널2 마디 단축을 위한 배열 : 기본값 1.0f
	
	inline std::string getBgmArray(int nIdx) { return m_aWav[nIdx]; }
	inline std::string getStageFileName() { return m_strStageFileName; }
	inline std::string getTitle() { return m_strTitle; }
	
	inline double getTotalTime() { return m_lTotalTime; }

	inline std::list<BMSNoteData*>* getBmsDataListPtr() { return &listBmsData; }
	inline std::list<BMSNoteData*>* getBgmDataListPtr() { return &listBgmData; }
private:

	void init();
	void clearListData();

	void parseBMSData(unsigned char* pBuffer, long lbufferSize); //파일에서 가져온 내용 분석
	void parseBMSLineData(std::string& strLineData); //라인별로 데이터 가져옴

	void addLongNote(BMSNoteData* pKeyData, int nType);
	void processLongNote();		//롱노트 처리
	
	EnumParserMode m_eParserMode;

	int m_nPlayer;

	std::string m_strGenre;
	std::string m_strTitle;
	std::string m_strArtist;

	float m_fBpm, Bpm;

	int m_nPlayLevel;
	int m_nRank;
	int m_nTotal;

	int m_nLNType;

	std::string m_strStageFileName;
	
	std::string m_aWav[MAXVALUE];
	std::string m_aBga[MAXVALUE];

	double m_aBpm[MAXVALUE];
	double m_aStop[MAXVALUE];

	bool m_aLNObj[MAXVALUE];

	double m_aLenthBeat[MAXVALUE];		//채널2 마디 단축을 위한 배열

	std::list<BMSNoteData*> listBmsData;
	std::list<BMSNoteData*> listBgaData;
	std::list<BMSNoteData*> listBgmData;
	std::list<BMSLNoteData*> listLNData;

	int m_nTotalNote;

	bool m_bHeaderOnly;

	long m_lTotalTime;

	inline EnumParserMode getParserMode() { return m_eParserMode; }
	inline void setParserMode(EnumParserMode eMode) { m_eParserMode = eMode; }

	inline int getPlayer() { return m_nPlayer; }
	inline void setPlayer(int nVal) { m_nPlayer = nVal; }

	inline std::string getGenre() { return m_strGenre; }
	inline void setGenre(std::string strVal) { m_strGenre = strVal; }
	
	inline void setTitle(std::string strVal) { m_strTitle = strVal; }
	inline std::string getArtist() { return m_strArtist; }
	inline void setArtist(std::string strVal) { m_strArtist = strVal; }
	
	inline float getBpm() { return m_fBpm; }
	inline void setBpm(float fVal) { m_fBpm = fVal; }

	inline int getPlayLevel() { return m_nPlayLevel; }
	inline void setPlayLevel(int nVal) { m_nPlayLevel = nVal; }
	inline int getRank() { return m_nRank; }
	inline void setRank(int nVal) { m_nRank = nVal; }
	inline int getTotal() { return m_nTotal; }
	inline void setTotal(int nVal) { m_nTotal = nVal; }

	inline int getLNType() { return m_nLNType; }
	inline void setLNType(int nVal) { m_nLNType = nVal; }

	inline void setStageFileName(std::string strVal) { m_strStageFileName = strVal; }

	inline void setTotalNote(int nVal) { m_nTotalNote = nVal; }

	inline bool getHeaderOnly() { return m_bHeaderOnly; }
	inline void setHeaderOnly(bool bVal) { m_bHeaderOnly = bVal; }

	inline void setTotalTime(long lVal) { m_lTotalTime = lVal; }
};

#endif // __BMS_PARSER_H__
