#include "BMSParser.h"
#include "iostream"
#include "fstream"
#include "algorithm"
#include "cocos2d.h"
#include "Util.h"

USING_NS_CC;

bool compare(BMSNoteData* a, BMSNoteData* b)
{
	return a->beat < b->beat;
}

CBMSParser::CBMSParser()
{
	m_eParserMode = EM_PARSER_MODE_NONE;
	m_bHeaderOnly = false;
}

CBMSParser::~CBMSParser()
{

}

void CBMSParser::init()
{
	m_nPlayer = 0;
	m_strGenre.clear();
	m_strTitle.clear();
	m_strArtist.clear();

	m_fBpm = 0.0f;

	m_nPlayLevel = 0;
	m_nRank = 3;			//easy
	m_nTotal = 0;

	m_nLNType = 1;			//default

	m_strStageFileName.clear();

	for (int i = 0; i<MAXVALUE; i++)
		m_aLNObj[i] = false;

	for (int i = 0; i<MAXVALUE; i++)
		m_aLenthBeat[i] = 1.0f;

	clearListData();
	
	m_nTotalNote = 0;
	m_lTotalTime = 0;
}

void CBMSParser::clearListData()
{
	std::list<BMSNoteData*>::iterator it = listBmsData.begin();
	std::list<BMSNoteData*>::iterator stop = listBmsData.end();
	while (it != stop){
		BMSNoteData* pData = *it;
		CC_SAFE_DELETE(pData);
		it++;
	}
	listBmsData.clear();
	
	it = listBgaData.begin();
	stop = listBgaData.end();
	while (it != stop){
		BMSNoteData* pData = *it;
		CC_SAFE_DELETE(pData);
		it++;
	}
	listBgaData.clear();

	it = listBgmData.begin();
	stop = listBgmData.end();
	while (it != stop){
		BMSNoteData* pData = *it;
		CC_SAFE_DELETE(pData);
		it++;
	}
	listBgmData.clear();

	std::list<BMSLNoteData*>::iterator it2 = listLNData.begin();
	std::list<BMSLNoteData*>::iterator stop2 = listLNData.end();
	while (it2 != stop2){
		BMSLNoteData* pData = *it2;
		CC_SAFE_DELETE(pData);
		it++;
	}
	listLNData.clear();
}

bool CBMSParser::loadBMSFile(std::string strFileName, bool bHeaderOnly/* = false*/)
{
	//std::string path = "/mnt/sdcard/kimImage/background2.jpg";
	setHeaderOnly(bHeaderOnly);

	std::string strDefaultPath;// = CCFileUtils::sharedFileUtils()->getWritablePath();
	strDefaultPath.append(strFileName);
	
	bool bFileOpenSuccess = false;
	ssize_t bufferSize = 0;
	unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(strDefaultPath.c_str(), "rb", &bufferSize);
	if (pBuffer)
	{
		//log("pBuffer size : %d", bufferSize);

		//BMS파싱한 데이터를 실제 게임 데이터로 변환
		parseBMSData(pBuffer, bufferSize);
		bFileOpenSuccess = true;
	}

	CC_SAFE_DELETE_ARRAY(pBuffer);
	
	return bFileOpenSuccess;
}

bool CBMSParser::getBMSHeader(BMSHeaderData& pHeaderData)
{
	bool bExistHeaderData = getHeaderOnly();
	if (bExistHeaderData)
	{
		pHeaderData.title = getTitle();
		pHeaderData.genre = getGenre();
		pHeaderData.artist = getArtist();

		pHeaderData.bpm = getBpm();

		pHeaderData.playlevel = getPlayLevel();
		pHeaderData.rank = getRank();
	}

	return bExistHeaderData;
}

void CBMSParser::parseBMSData(unsigned char* pBuffer, long lbufferSize)
{
	std::string strData((const char*)pBuffer, lbufferSize);

	//초기화
	init();

	//개행별로 문자열 분리
	CStrTokenizer token(strData, "\r\n");
	while (token.hasToken())
	{
		//log("token : %s, token size : %d", token.nextToken().c_str(), token.nextToken().size());
		std::string strTrim = Trim(token.nextToken());
		parseBMSLineData(strTrim);
	}

	//데이터 소팅
	if (getHeaderOnly() == false)
	{
		listBmsData.sort(compare);
		listBgaData.sort(compare);
		listBgmData.sort(compare);
		processLongNote();
	}
	
	if (getTotal() == 0)
	{
		int nTotal = (getTotalNote()*0.16f) + 160;
		setTotal(nTotal);
	}
}

void CBMSParser::parseBMSLineData(std::string& strLineData)
{
	///전처리 미지원

	//각각 어떤 데이터인지
	if (strLineData.compare("*---------------------- HEADER FIELD") == 0) {
		setParserMode(EM_PARSER_MODE_HEADER);
		log("+========= HEADER FIELD");
		return;
	}
	if (strLineData.compare("*---------------------- MAIN DATA FIELD") == 0) {
		setParserMode(EM_PARSER_MODE_MAINDATA);
		log("+========= MAIN DATA FIELD");
		return;
	}
	if (strLineData.compare("*---------------------- BGA FIELD") == 0) {
		//미지원
		return;
	}

	//헤더 데이터
	if (getParserMode() == EM_PARSER_MODE_HEADER)
	{
		std::vector<std::string> vecSplitStr;
		vecSplitStr.clear();
		Split(strLineData, " ", vecSplitStr, 2);

		if (vecSplitStr.size() > 1)
		{
			if (vecSplitStr.at(0).compare("#PLAYER") == 0) 
			{
				//int nPlayer = std::stoi(vecSplitStr.at(1));
				int nPlayer = atoi(vecSplitStr.at(1).c_str());
				setPlayer(nPlayer);
				log("+= #PLAYER : %d", getPlayer());
			}
			else if (vecSplitStr.at(0).compare("#GENRE") == 0)
			{
				setGenre(vecSplitStr.at(1));
				log("+= #GENRE : %s", getGenre().c_str());
			}
			else if (vecSplitStr.at(0).compare("#TITLE") == 0)
			{
				setTitle(vecSplitStr.at(1));
				log("+= #TITLE : %s", getTitle().c_str());
			}
			else if (vecSplitStr.at(0).compare("#ARTIST") == 0)
			{
				setArtist(vecSplitStr.at(1));
				log("+= #ARTIST : %s", getArtist().c_str());
			}
			else if (vecSplitStr.at(0).compare("#BPM") == 0)
			{
				//float fBpm = std::stof(vecSplitStr.at(1));
				float fBpm = atof(vecSplitStr.at(1).c_str());
				setBpm(fBpm);
				log("+= #BPM : %f", getBpm());
			}
			else if (vecSplitStr.at(0).compare("#PLAYLEVEL") == 0)
			{
				//int nPlayLevel = std::stoi(vecSplitStr.at(1));
				int nPlayLevel = atoi(vecSplitStr.at(1).c_str());
				setPlayLevel(nPlayLevel);
				log("+= #PLAYLEVEL : %d", getPlayLevel());
			}
			else if (vecSplitStr.at(0).compare("#RANK") == 0)
			{
				//int nRank = std::stoi(vecSplitStr.at(1));
				int nRank = atoi(vecSplitStr.at(1).c_str());
				setRank(nRank);
				log("+= #RANK : %d", getRank());
			}
			else if (vecSplitStr.at(0).compare("#STAGEFILE") == 0)
			{
				setStageFileName(vecSplitStr.at(1));
				log("+= #STAGEFILE : %s", getArtist().c_str());
			}
			else if (vecSplitStr.at(0).compare("#DIFFICULTY") == 0)
			{
				//미지원
			}
			else if (vecSplitStr.at(0).compare("#VOLWAV") == 0)
			{
				//미지원
			}
			else if (vecSplitStr.at(0).compare("#LNTYPE") == 0)
			{
				//int nLNType = std::stoi(vecSplitStr.at(1));
				int nLNType = atoi(vecSplitStr.at(1).c_str());
				setLNType(nLNType);
				log("+= #LNTYPE : %d", getLNType());
			}
			else if (vecSplitStr.at(0).compare("#STP") == 0)
			{
				//미지원
			}
			else if (vecSplitStr.at(0).compare("#LNOBJ") == 0)
			{
				//미지원
			}
			else if (vecSplitStr.at(0).find("#BMP") != std::string::npos)
			{
				//int nIndex = std::stoi(vecSplitStr.at(0).substr(4, 2));
				int nIndex = atoi(vecSplitStr.at(0).substr(4, 2).c_str());
				m_aBga[nIndex] = vecSplitStr.at(1);
				log("+= #BMP%02d : %s", nIndex, m_aBga[nIndex].c_str());
			}
			else if (vecSplitStr.at(0).find("#WAV") != std::string::npos)
			{
				//int nIndex = std::stoi(vecSplitStr.at(0).substr(4, 2));
				int nIndex = atoi(vecSplitStr.at(0).substr(4, 2).c_str());
				std::string strFileName = vecSplitStr.at(1);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				ReplaceString(strFileName, "mp3", "wav");	
#endif
				m_aWav[nIndex] = strFileName;
				log("+= #WAV%02d : %s", nIndex, m_aWav[nIndex].c_str());
			}
			else if (vecSplitStr.at(0).find("#BPM") != std::string::npos)
			{
				//int nIndex = std::stoi(vecSplitStr.at(0).substr(4, 2));
				int nIndex = atoi(vecSplitStr.at(0).substr(4, 2).c_str());
				//m_aBpm[nIndex] = std::stod(vecSplitStr.at(1));
				m_aBpm[nIndex] = atof(vecSplitStr.at(1).c_str());
				log("+= #BPM%02d : %lf", nIndex, m_aBpm[nIndex]);
			}
			else if (vecSplitStr.at(0).find("#STOP") != std::string::npos)
			{
				//int nIndex = std::stoi(vecSplitStr.at(0).substr(4, 2));
				int nIndex = atoi(vecSplitStr.at(0).substr(4, 2).c_str());
				//m_aStop[nIndex] = std::stod(vecSplitStr.at(1));
				m_aStop[nIndex] = atof(vecSplitStr.at(1).c_str());
				log("+= #STOP%02d : %lf", nIndex, m_aStop[nIndex]);
			}
		}
	}

	//메인 데이터
	if (getParserMode() == EM_PARSER_MODE_MAINDATA)
	{
		std::vector<std::string> vecSplitStr;
		vecSplitStr.clear();
		Split(strLineData, ":", vecSplitStr, 2);

		if (vecSplitStr.at(0).size() < 6)
			return;

		if (IsInteger(vecSplitStr.at(0).substr(1, 6).c_str()) == false)
			return;		//#이후에 숫자가 아니면 리턴

		//int nMadi = std::stoi(vecSplitStr.at(0).substr(1, 3));
		//int nChannel = std::stoi(vecSplitStr.at(0).substr(4, 2));
		int nMadi = atoi(vecSplitStr.at(0).substr(1, 3).c_str());
		int nChannel = atoi(vecSplitStr.at(0).substr(4, 2).c_str());

		if (nChannel == 2) 
		{
			if (getHeaderOnly() == false)
			{
				//마디 단축 채널
			}
		}
		else
		{
			int nNoteLength = vecSplitStr.at(1).length();	//마디 내 노트 길이
			for (int i = 0; i < nNoteLength / 2; i++)
			{
				std::string strVal = vecSplitStr.at(1).substr((i * 2), 2);
				int nVal = hexToInt(strVal.c_str());
				
				if (nVal == 0)		//0은 사용하지 않음
					continue;

				//마디 내 비트
				double dNoteBeat = nMadi + (static_cast<double>(i) / static_cast<double>(nNoteLength/2));

				if (getHeaderOnly() == false)
				{
					BMSNoteData* pKeyData = new BMSNoteData();
					pKeyData->value = nVal;
					pKeyData->key = nChannel;
					pKeyData->beat = dNoteBeat;
					pKeyData->index = m_nTotalNote;

					//11 to 19 : 1 플레이어 노트정보 (1번키,2번키,3번키,4번키,5번키,스크래치,프리존(비사용),6번키,7번키)
					if (nChannel > 10 && nChannel < 20)
						m_nTotalNote++;

					/*채널 정보
					channel #01: 배경음 채널지정한 키값을 배경음으로 재생한다. 이 배경음은 어떤 경우에라도 항상 재생된다. 존재하지 않는 키값은 무시한다.
					channel #02: 마디 단축이 채널은 다른 채널과는 다르게 데이터로 숫자를 받는다. 지정한 실수 배로 해당 마디의 길이를 줄이거나 늘린다. (0.5일 경우 원래 길이의 반으로, 2.0일 경우 원래 길이의 두 배로) 이는 BPM의 조정과 다르며, 해당 마디가 재생되는 시간 자체가 그만큼 짧아지거나 길어지는 것을 의미한다.
					channel #03: BPM 채널해당하는 위치에서, BPM을 지정한 16진수 숫자(1부터 255까지)로 바꾼다. 예를 들어서 78은 BPM 120을 의미한다.
					channel #04: BGA 채널현재 표시되고 있는 배경 이미지 파일을 해당하는 키값에 지정된 이미지 파일로 바꾼다. 지정된 이미지는 다른 이미지가 이 채널에서 지정될 때까지 계속 보여진다.
					channel #05: BM98 확장 채널이 채널은 BM98에서만 사용할 수 있다. 따라 오는 데이터들을 순서대로 해당 마디의 큐에 넣는다. 그리고 오브젝트 채널에서 명령을 내릴 때, 해당 마디의 큐가 비어 있지 않으면 나오는 순서대로 00이 아닌 키값에 큐에서 키값을 꺼내서 그 키값에 대응하는 스프라이트를 기본 오브젝트 모양 대신 출력한다. 키값에 대응하는 스프라이트 번호는 미리 지정되어 있으며 부록에 그 목록이 있다.
					channel #06: Poor BGA 채널사용자가 오브젝트를 입력하지 못 했을 때 표시될 배경 이미지를 해당 키값의 이미지로 바꾼다. 일반적인 BGA 채널과는 달리 Poor BGA 채널은 하나 뿐이며, 투명색 등은 모두 무시된다.
					channel #07: BGA 레이어 채널현재 표시되고 있는 배경 이미지 파일을 해당하는 키값에 지정된 이미지 파일로 바꾼다. channel #04와 다른 점은 이 이미지는 channel #04에서 지정한 이미지 위에 표시된다는 것이다. (검은색, 즉 투명색으로 되어 있는 부분은 아래의 일반 BGA 이미지가 비쳐 보이게 된다.) 그래픽 편집 툴의 레이어 기능과 흡사하다.
					channel #08: 확장 BPM 채널해당하는 위치에서, BPM을 지정한 키값에 지정된 BPM 값(#BPMxx 명령으로 지정한 값)으로 변경한다.
					channel #09: 시퀀스 정지 채널해당하는 위치에서, 지정한 키값에 지정된 시간(#STOPxx 명령으로 지정한 값) 만큼 오브젝트가 움직이는 것을 멈춘다. 이미 재생되던 배경음은 그대로 재생되지만 다른 모든 것은 해당하는 시간만큼 멈춰 있게 된다.
					*/
					switch (nChannel)
					{
					case 1:
						listBgmData.push_back(pKeyData);
						break;
					case 3:
						//pKeyData->value = std::stoi(strVal, 0, 16);
						pKeyData->value = hexToInt(strVal.c_str());
						listBmsData.push_back(pKeyData);
						break;
					case 4:
					case 6:
					case 7:
						listBgaData.push_back(pKeyData);
						break;
					case 8:
						pKeyData->value = m_aBpm[nVal];
						listBmsData.push_back(pKeyData);
						break;
					case 9:
						pKeyData->value = m_aStop[nVal];
						listBmsData.push_back(pKeyData);
						break;
					case 11:	// 1 플레이어 노트 정보
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:	// 7 프리존(비사용)
					case 18:
					case 19:
						//LNObj 체크는 여기서
						//현재는 미지원
						listBmsData.push_back(pKeyData);
						break;
					case 31:	// 1 플레이어 투명노트 정보
					case 32:
					case 33:
					case 34:
					case 35:
					case 36:	// 7 프리존(비사용)
					case 38:
					case 39:
						listBmsData.push_back(pKeyData);
						break;
					case 51:
					case 52:
					case 53:
					case 54:
					case 55:
					case 56:
					case 58:
					case 59:
						// 롱노트
						addLongNote(pKeyData, getLNType());
						break;
					}
				}
			}
		}

		log("+= #%03d%02d : %s", nMadi, nChannel, vecSplitStr.at(1).c_str());
	}
}

double CBMSParser::getBPMArray(int nKeyValue)
{
	return m_aBpm[nKeyValue];
}

double CBMSParser::getSTOPArray(int nKeyValue)
{
	return m_aStop[nKeyValue];
}

double CBMSParser::getBeatFromTime(long lMillisec)
{
	//재생 시간으로부터 비트를 가져옴
	double dBpm = static_cast<double>(getBpm());
	double dBeat = 0;

	// 정확성을 높이기 위해 double형
	double dTime = 0;
	double dNewtime = 0;

	auto it = listBmsData.begin();
	auto stop = listBmsData.end();

	for (; it != stop; it++)
	{
		BMSNoteData* pData = *it;

		// 비트 구간에 대한 시간 합산
		while (pData->beat > static_cast<int>(dBeat) + 1)
		{
			dNewtime = dTime + ((dBeat + 1) - dBeat) * (1.0f / dBpm * 60 * 4) * 1000 * getLengthBeat(static_cast<int>(dBeat));
			if (dNewtime >= lMillisec)
			{
				double dReturnBeat = dBeat + (lMillisec - dTime) * (dBpm / 60000 / 4.0f) / getLengthBeat(static_cast<int>(dBeat));
				return dReturnBeat;
			}

			dTime = dNewtime;
			dBeat = static_cast<int>(dBeat) + 1;
		}
		
		if (pData->key == 9) 
		{	// STOP
			dTime += pData->value * 1000;
			if (dTime >= lMillisec)
				return dBeat;
			continue;
		}

		if (pData->key == 3 || pData->key == 8) 
		{
			//새 BPM으로 계산된 시간
			dNewtime = dTime + (pData->beat - dBeat) * (1.0f / dBpm * 60 * 4) * 1000 * getLengthBeat(static_cast<int>(dBeat));
			if (dNewtime >= lMillisec)
			{
				double dReturnBeat = dBeat + (lMillisec - dTime)*(dBpm / 60000 / 4.0f) / getLengthBeat(static_cast<int>(dBeat));
				return dReturnBeat;
			}

			dBeat = pData->beat;
			dBpm = pData->value;
			dTime = dNewtime;
		}
	}

	dBeat += (lMillisec - dTime)*(static_cast<double>(dBpm / 60000 / 4.0f));

	return dBeat;
}
double CBMSParser::getBPMFromBeat(double dBeat)
{
	//변속 BPM 혹은 확장 BPM에 쓰임
	double dBpm = static_cast<double>(getBpm());

	auto it = listBmsData.begin();
	auto stop = listBmsData.end();

	for (; it != stop; it++)
	{
		BMSNoteData* pData = *it;
		if (pData->beat > dBeat)
			break;
		if (pData->key == 3 || pData->key == 8)
			dBpm = pData->value;
	}

	return dBpm;
}

void CBMSParser::setNoteTime()
{
	//노트 데이터에 시간 정보를 심어줌
	double dBpm = static_cast<double>(getBpm());
	double dTime = 0;
	double dBeat = 0;

	auto it = listBmsData.begin();
	auto stop = listBmsData.end();

	for (; it != stop; it++)
	{
		BMSNoteData* pData = *it;
		
		//비트 구간에 대한 시간 합산

		while (pData->beat >= static_cast<int>(dBeat)+1)
		{
			dTime += (static_cast<int>(dBeat)+1 - dBeat) * (1.0f / dBpm * 60 * 4) * getLengthBeat(static_cast<int>(dBeat));
			dBeat = static_cast<int>(dBeat)+1;
		}

		dTime += (pData->beat - dBeat) * (1.0f / dBpm * 60 * 4) * getLengthBeat(static_cast<int>(dBeat));
		pData->time = dTime * 1000;	// 밀리세컨

		if (pData->key == 3 || pData->key == 8)		//BPM변속일 경우 BPM 갱신
			dBpm = pData->value;

		if (pData->key == 9)							//STOP일경우 Time에 합산
			dTime = pData->value;

		dBeat = pData->beat;
	}

	setTotalTime(dTime);
}

void CBMSParser::addLongNote(BMSNoteData* pKeyData, int nType)
{
	m_nTotalNote++;
	pKeyData->key = pKeyData->key % 10 + 50;
	pKeyData->attr = nType;
	listBmsData.push_back(pKeyData);
}

void CBMSParser::processLongNote()
{
	//반드시 노트 정렬 후에 처리해야함
	auto it = listBmsData.begin();
	auto stop = listBmsData.end();

	for (; it != stop; it++)
	{
		BMSNoteData* pData = *it;
		if (pData->key > 50 && pData->key < 60)
		{
			if (pData->attr == 1)
			{
				// LNTYPE 1
				auto it2 = it;
				auto stop2 = listBmsData.end();
				while (it2 != stop2)
				{
					++it2;
					BMSNoteData* pLNData = *it2;
					if (pLNData->key == pData->key && pLNData->attr == 1)
					{
						pLNData->attr = 4;	// 롱노트 끝 4
						m_nTotalNote++;
						break;
					}
				}
			}
			else if (pData->attr = 2)
			{
				// LNTYPE 2 미지원
			}
		}
	}
}

int CBMSParser::getTotalMadiCount()
{
	int nCount = 0;

	auto it = listBmsData.begin();
	auto stop = listBmsData.end();

	for (auto& item : listBmsData)
	{
		auto pData = item;
		if (std::fmod(pData->beat, 1) == 0)
		{
			int nBeat = static_cast<int>(pData->beat);
			if (nBeat > nCount)
				nCount = nBeat;
		}
	}

	/*for (; it != stop; it++)
	{
		BMSNoteData* pData = *it;
		if (std::fmod(pData->beat, 1) == 0)
			nCount++;
	}*/
	
	return nCount;
}

BMSNoteData* CBMSParser::getValidNoteData(int nKey)
{
	auto it = listBmsData.begin();
	auto stop = listBmsData.end();

	for (auto& item : listBmsData)
	{
		auto pData = item;
		
		//해당 키 채널과 맞고 일반노트(10~20), 롱노트(50~60)이고 아직 처리 되지 않은 노트 반환
		if (pData->key % 10 == nKey &&
			((pData->key > 10 && pData->key < 20) || (pData->key > 50 && pData->key < 60)) &&
			pData->attr == 0)
		{
			return pData;
		}
	}

	return nullptr;
}
