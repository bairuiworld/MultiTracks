#ifndef __WW_TREEVIEW_H__
#define __WW_TREEVIEW_H__

#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include "Widget.h"
#include "SimpleSignal.h"

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
	static TreeNode* FromHandle(HWND hTreeView, HTREEITEM item);

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
	virtual void OnNotify(LPNMHDR lpnmhdr);

	virtual void OnSelChanged(TreeNode* newNode, TreeNode* oldNode) {}

protected:
	TreeNode mRootNode;

public:
	sig::Signal<void(TreeNode*, TreeNode*)> SignalSelChanged;
};

}

#endif // !__WW_TREEVIEW_H__