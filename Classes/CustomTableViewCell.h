#ifndef __CUSTOM_TABLE_VIEW_CELL_H__
#define __CUSTOM_TABLE_VIEW_CELL_H__

#include "cocos2d.h"
#include "extensions\cocos-ext.h"

class CustomTableViewCell : public cocos2d::extension::TableViewCell
{
public:
	enum {
		kTagCellTextTag = 123,
		kTagCellBg,
	};

	void setText(std::string strText);
	void setSelect(bool bIsSelect);
};

#endif // __CUSTOM_TABLE_VIEW_CELL_H__
