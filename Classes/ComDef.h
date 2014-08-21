#ifndef __COM_DEF_H__
#define __COM_DEF_H__

//getter/setter 매크로
#define PR_PROPERTY(varType, varName, funName)\
private: varType varName; \
public: inline varType get##funName(void); \
public: inline void set##funName(varType var);

#define PR_ARRAY_PROPERTY (varType , varName , funName , arraySize )\
private: varType varName[arraySize]; \
public: inline varType get##funName(int i){ return varName[i]; }\
public: inline void set##funName(int i, varType var){ varName[i] = var; }


enum {
	UI_DEPTH_BOTTOM = 0,
	UI_DEPTH_BOTTOM_1,
	UI_DEPTH_BOTTOM_2,
	UI_DEPTH_MIDDLE,
	UI_DEPTH_TOP,
	UI_DEPTH_TOP_1,
	UI_DEPTH_TOP_2,
};

typedef struct tagCallInfo
{
	void* pWhoCallPtr;					/// 클래스 포인터
	int nCallTag;						/// 태그값
	int nParam1;						/// 파라미터1
	int nParam2;						/// 파라미터2
}stCallInfo;

class CKeyboardEventDelegate {
public:
	virtual void onKeyboardEvent(int nKeyIndex, bool bShow) = 0;
};

enum EnumJudgeType{
	EM_JUDGE_PERFECT = 0,
	EM_JUDGE_GOOD,
	EM_JUDGE_MISS,
	EM_JUDGE_MAX
};

#endif // __COM_DEF_H__