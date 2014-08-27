#include <Windows.h>
#include <CommCtrl.h>
#include "TreeView.h"

namespace ww
{

TreeNode::TreeNode(const std::string& text) :
mTreeView(nullptr), mParent(nullptr), mHandle(nullptr), mText(text)
{

}

TreeNode::~TreeNode()
{
	for(TreeNode* child : mChildren)
		delete child;
}

TreeNode* TreeNode::FromHandle(HWND hTreeView, HTREEITEM item)
{
	TVITEMEX tvi;
	ZeroMemory(&tvi, sizeof(TVITEMEX));
	tvi.mask = TVIF_PARAM;
	tvi.hItem = item;
	TreeView_GetItem(hTreeView, &tvi);
	return reinterpret_cast<TreeNode*>(tvi.lParam);
}

bool TreeNode::AddNode(TreeNode* node)
{
	if(node->mParent != nullptr) return false;
	node->mParent = this;
	mChildren.push_back(node);
	AddNodeToTreeView(node);
	return true;
}

void TreeNode::AddNodeToTreeView(TreeNode* node)
{
	if(mTreeView == nullptr) return;
	if(mTreeView->GetHandle() == nullptr) return;
	node->mTreeView = mTreeView;

	TVINSERTSTRUCT is;
	is.hParent = mHandle;
	is.hInsertAfter = TVI_LAST;
	is.item.mask = TVIF_TEXT | TVIF_PARAM;
	is.item.pszText = const_cast<LPSTR>(node->mText.c_str());
	is.item.lParam = reinterpret_cast<LPARAM>(node);

	node->mHandle = TreeView_InsertItem(mTreeView->GetHandle(), &is);

	for(TreeNode* child : node->mChildren)
		node->AddNodeToTreeView(child);
}

TreeView::TreeView() :
Widget(WC_TREEVIEW, "", TVS_FULLROWSELECT | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS)
{
	mRootNode.mTreeView = this;
}

void TreeView::Create(Widget* parent)
{
	Widget::Create(parent);
	for(TreeNode* child : mRootNode.mChildren)
		mRootNode.AddNodeToTreeView(child);
}

bool TreeView::AddNode(TreeNode* node)
{
	return mRootNode.AddNode(node);
}

void TreeView::OnNotify(LPNMHDR lpnmhdr)
{
	switch(lpnmhdr->code)
	{
	case TVN_SELCHANGED:
	{
		LPNMTREEVIEW lpnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
		TreeNode* newNode = reinterpret_cast<TreeNode*>(lpnmtv->itemNew.lParam);
		TreeNode* oldNode = reinterpret_cast<TreeNode*>(lpnmtv->itemNew.lParam);
		SignalSelChanged.emit(newNode, oldNode);
		OnSelChanged(newNode, oldNode);
	} break;
	}
}

}