#include "CustomTableViewCell.h"
#include "ComDef.h"

USING_NS_CC;

void CustomTableViewCell::setText(std::string strText)
{
	auto label = LabelTTF::create(strText, "Helvetica", 20.0f);
	label->setColor(Color3B::BLACK);
	label->setAnchorPoint(Vec2::ZERO);
	label->setPosition(Vec2(10, 30));
	this->addChild(label, UI_DEPTH_BOTTOM_1, kTagCellTextTag);

	setSelect(false);
}

void CustomTableViewCell::setSelect(bool bIsSelect)
{
	std::string path;
	if (bIsSelect)
	{
		path = "Images/Select_Cell.png";
	}
	else
	{
		path = "Images/Default_Cell.png";
	}

	auto pBgSprite = Sprite::create(path);
	pBgSprite->setPosition(Vec2::ZERO);
	pBgSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(pBgSprite, UI_DEPTH_BOTTOM, kTagCellBg);
	
}