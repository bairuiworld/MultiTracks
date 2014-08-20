#ifndef __WW_TREEVIEW_H__
#define __WW_TREEVIEW_H__

#include <string>
#include "Widget.h"

namespace ww
{

class TreeView;

class TreeNode
{
	friend TreeView;
	using TreeNodeList = std::vector<TreeNode*>;

public:
	TreeNode(const std::string& text = "");
	virtual ~TreeNode();

	TreeView* GetTreeView() const { return mTreeView; }
	HTREEITEM GetHandle() const { return mHandle; }

	bool AddNode(TreeNode* node);

protected:
	void AddNodeToTreeView(TreeNode* node);

protected:
	TreeView* mTreeView;
	TreeNode* mParent;
	HTREEITEM mHandle;
	std::string mText;
	TreeNodeList mChildren;
};

class TreeView : public Widget
{
public:
	TreeView();

	bool AddNode(TreeNode* node);

protected:
	virtual void Create(Widget* parent);

protected:
	TreeNode mRootNode;
};

}

#endif // !__WW_TREEVIEW_H__