#ifndef __UTIL_H__
#define __UTIL_H__
///토크나이저 출처
///http://cafe.naver.com/cocos2dxusers/3698
///
#include <string>
class CStrTokenizer
{
private:
	int m_nIndex;
	int m_nPos;
	std::string m_strDelim;
	const std::string& m_strTarget;
public:
	CStrTokenizer(const std::string & target, const std::string & d) : m_strTarget(target), m_strDelim(d)
	{
		m_nPos = m_nIndex = 0;
	}
	bool hasToken()
	{
		return m_nPos != std::string::npos && m_nPos != m_strTarget.size() - 1;
	}
	std::string nextToken()
	{
		m_nPos = m_strTarget.find(m_strDelim, m_nIndex);
		std::string ret = m_strTarget.substr(m_nIndex, m_nPos - m_nIndex);
		m_nIndex = m_nPos + 1;
		return ret;
	}
};

///Trim 출처
///http://blog.naver.com/mao0524/140035069639
///
std::string Trim(const std::string& s)
{

	unsigned int f, e;

	if (s.length() == 0)
		return s;

	f = s.find_first_not_of(" \t\r\n");
	e = s.find_last_not_of(" \t\r\n");

	if (f == std::string::npos)
		return "";
	return std::string(s, f, e - f + 1);
}

//split
void Split(const std::string& target, const std::string& delim, std::vector<std::string>& vecSplit, int nCount/* = -1*/)
{
	int nIndex = 0;
	int nPos = 0;	

	//구분자로 n등분함
	for (int i = 0; i < nCount; i++)
	{
		std::string ret;
		ret.clear();

		if (i == nCount - 1)
		{
			//n등분 완료
			ret = target.substr(nIndex, std::string::npos);
		}
		else
		{
			nPos = target.find(delim, nIndex);
			ret = target.substr(nIndex, nPos - nIndex);
			nIndex = nPos + 1;
		}

		vecSplit.push_back(ret);
	}
}

bool IsInteger(const char* _Str)
{
	int i = 0;

	if (_Str[i] == '-' || _Str[i] == '+')       // 사인 확인
		++i;

	for (; _Str[i] != 0; ++i)
	{
		if (_Str[i] < '0' || _Str[i] > '9')
			return false;
	}

	return true;

}

/*
* 이 함수는 파라메터로 받은 16진수 문자열을 정수값으로
* 반환한다. 문자열의 길이가 정수형의 최대값을 넘게 될 경우
* overflow 가 발생하므로 주의.
*
* 또, 초기화 되지 않은 문자열을 넘길경우 access-violation이
* 발생할 수 도 있다.
*/
int hexToInt(const char* szHex) {
	int hex = 0; // 반환될 값. 초기에는 0이다.
	int nibble;  // 16진수의 한 니블(4비트)값을 담아둘 곳

	// while 루프를 포인터 szHex가 가리키는 곳의 값이 0일 때
	// 까지 돈다. C언어의 문자열은 항상 NULL termination 형태
	// 이므로, 문자열의 마지막을 포인터가 가리킨다면 그 값은
	// '\0' 즉 0이다. 0은 비교문에서 FALSE를 의미하므로
	// while 루프를 탈출하게 된다.
	while (*szHex) {
		// 현재 계산된 16진수 변환 값을 4비트 왼쪽으로 시프트
		// 시킨다. 이유는 다음 4비트를 읽어서 넣기 위해서 이다.
		//
		// 예제로 주어진 c0cb900b 값을 읽게 될때 hex의 변화는
		// 아래와 같다.
		//
		// iteration 1 : [00000000]
		// iteration 2 : [0000000c] --LSHIFT--> [000000c0]
		// iteration 3 : [000000c0] --LSHIFT--> [00000c00]
		// iteration 4 : [00000c0c] --LSHIFT--> [0000c0c0]
		// iteration 5 : [0000c0cb] --LSHIFT--> [000c0cb0]
		// iteration 6 : [000c0cb9] --LSHIFT--> [00c0cb90]
		// iteration 7 : [00c0cb90] --LSHIFT--> [0c0cb900]
		// iteration 8 : [0c0cb900] --LSHIFT--> [c0cb9000]
		// ----
		// iteration 8 에서 아래의 nibble 을 OR 연산한 결과는
		// iteration 8 result : [c0cb900b] 가 되어 원하는 값이
		// 들어있음을 알 수 있다.
		hex <<= 4;

		if (*szHex >= '0' && *szHex <= '9') {
			// 현재 szHex 포인터가 가리키는 곳의 문자가 0~9에
			// 해당하는 문자라면, '0' 을 빼서 숫자로 변환 한다.
			// ASCII 코드상 문자 '0' ~ '9' 는 순차적으로 배치되어
			// 있으므로, 문자 '0'의 ASCII 코드 값을 빼 줌으로서
			// 숫자값으로 변환할 수 있다.
			nibble = *szHex - '0';
		}
		else if (*szHex >= 'a' && *szHex <= 'f') {
			// 현재 szHex 포인터가 가리키는 곳의 문자가 a~f에
			// 해당하는 문자라면, 'a' 를 뺀후 10을 더하여 숫자로
			// 변환 한다. ASCII 코드상 문자 'a' ~ 'f'  는 순차적으로
			// 배치되어 있으므로, 문자 'a' 의 ASCII 코드 값을
			// 빼 줌으로서 0 ~ 5 라는 숫자값으로 변환할 수 있다.
			// 여기에 16진수 a 는 10 진수 10 을 말하는 것이므로,
			// 10을 더하여 'a' ~ 'f' 를 0x0a ~ 0x0f 값으로 변환
			// 해 준다.
			nibble = *szHex - 'a' + 10;
		}
		else if (*szHex >= 'A' && *szHex <= 'F') {
			// 소문자 'a' ~ 'f' 와 동일 하므로 설명 생략
			nibble = *szHex - 'A' + 10;
		}
		else {
			// '0' ~ '9' 혹은 'a' ~ 'f'('A' ~ 'F') 외의 문자가
			// 들어 있다면 무시하고 nibble의 값을 0으로 세팅한다.
			nibble = 0;
		}

		// 위에서 구한 nibble 값을 지금까지 구한 hex 값에 비트연산 OR를
		// 시킴으로서 그 결과를 더한다. 그냥 더하기를 해줘도 상관 없을
		// 듯 하지만(이미 위에서 hex <<= 4 라는 문장을 통해 하위 4비트를
		// 비워 놨으므로 더하기를 한다고 하여 자리 올림 등이 발생할 거
		// 같지는 않다..) OR를 한다고 해서 큰일 나는것도 아니기 때문에
		// 그냥 OR 연산을 취했다.
		//
		// iteration 5 의 예를 들어보면 이때 구해진 hex는 현재 왼쪽 시프트
		// 한 결과로
		//
		// [000c0cb0] 란 값이 hex에 들어있다. 여기에 방금 구한 nibble 값인
		// [00000009] 를 OR 연산을 취하면 결과는
		// [000c0cb9] 가 되어 정상적인 값이 됨을 알 수 있다.
		hex |= nibble;

		// szHex 포인터를 1 증가 시켜 다음 문자를 처리할 준비를 한다.
		szHex++;
	}

	// 구해진 정수 값을 리턴
	return hex;
}

void ReplaceString(std::string & strCallId, const char * pszBefore, const char * pszAfter)
{
	size_t iPos = strCallId.find(pszBefore);
	size_t iBeforeLen = strlen(pszBefore);

	while (iPos < std::string::npos)
	{
		strCallId.replace(iPos, iBeforeLen, pszAfter);
		iPos = strCallId.find(pszBefore, iPos);
	}
}

#endif // __UTIL_H__