#ifndef __KEY_PAD_H__
#define __KEY_PAD_H__

#include "cocos2d.h"
#include "ComDef.h"

//단순 키 일반/눌림 이미지 표현
class CKeyNode : public cocos2d::Node
{
	enum {
		kTagNormal = 0,
		kTagSelect,
	};
public:
	enum EnumKeyType{
		EM_RED_TYPE = 0,
		EM_BLUE_TYPE,
		EM_WHITE_TYPE,
		EM_BLACK_TYPE
	};

	CKeyNode();
	~CKeyNode();

	static CKeyNode* create(EnumKeyType eType, int nKeyNumber, void* pWhoCall = NULL, cocos2d::SEL_CallFuncND pFunc = NULL);

	void setNormal();
	void setSelect(int nTouchID = -1);

	inline int getTouchID() { return m_nTouchID; }
	inline bool getIsSelect() { return m_bIsSelect; }
	
private:

	bool init(EnumKeyType eType, int nKeyNumber);
	
	bool m_bIsSelect;
	int m_nKeyNum;
	int m_nTouchID;

	EnumKeyType m_eType;
	stCallInfo m_cCallInfo;
	cocos2d::SEL_CallFuncND m_pCallFunc;

	inline int getKeyNumber() { return m_nKeyNum; }
	inline void setKeyNumber(int nNum) { m_nKeyNum = nNum; }

	inline void setTouchID(int nTouch) { m_nTouchID = nTouch; }
	inline void setIsSelect(bool bSelect) { m_bIsSelect = bSelect; }

	inline EnumKeyType getType() { return m_eType; }
	inline void setType(EnumKeyType eType) { m_eType = eType; }

	inline stCallInfo getCallInfo() { return m_cCallInfo; }
	inline void setCallInfo(stCallInfo cCallInfo) { m_cCallInfo = cCallInfo; }

	inline cocos2d::SEL_CallFuncND getCallFunc() { return m_pCallFunc; }
	inline void setCallFunc(cocos2d::SEL_CallFuncND pCallFunc) { m_pCallFunc = pCallFunc; }
};

//키패드 레이어
class CKeypadLayer : public cocos2d::Layer
{
	enum{
		kTagKeypadButton1 = 0,
		kTagKeypadButton2,
		kTagKeypadButton3,
		kTagKeypadButton4,
		kTagKeypadButton5,
		kTagKeypadButton6,
		kTagKeypadButtonMax,

		kTagKeyboardButton1,
		kTagKeyboardButton2,
		kTagKeyboardButton3,
		kTagKeyboardButton4,
		kTagKeyboardButton5,
		kTagKeyboardButton6,
		kTagKeyboardButtonMax,
	};

	enum EnumTouchType
	{
		EM_TOUCH_BEGAN = 0,
		EM_TOUCH_MOVED,
		EM_TOUCH_ENDED
	};
public:

	static CKeypadLayer* create(CKeyboardEventDelegate* pDelegate = NULL);

	inline CKeyboardEventDelegate* getDelegate() { return m_pDelegate; }
	inline void setDelegate(CKeyboardEventDelegate* pDelegate) { m_pDelegate = pDelegate; }

	void onCheckKeyboardEvent(int nIndex, bool bIsPress);
private:

	CKeyboardEventDelegate* m_pDelegate;

	bool m_bIsOnKeyPress;

	inline bool getIsOnKeyPress() { return m_bIsOnKeyPress; }
	inline void setIsOnKeyPress(bool bVal) { m_bIsOnKeyPress = bVal; }

	bool init(CKeyboardEventDelegate* pDelegate = NULL);
	virtual void onEnter();
	virtual void onExit();
	virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	void checkTouch(cocos2d::Touch* touch, EnumTouchType eTouchType);

	void onKeyCallBack(Node* pNode, void* pPtr);
	void onKeyEvent(int nParam1, int nParam2);
};

#endif // __KEY_PAD_H__
