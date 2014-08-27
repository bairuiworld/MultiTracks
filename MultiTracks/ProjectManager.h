#ifndef __MULTITRACKS_PROJECTMANAGER_H__
#define __MULTITRACKS_PROJECTMANAGER_H__

#include "TreeView.h"

namespace mt
{

enum class ProjectNodeType { Track, Group, Project, Database };
template <class T> struct ProjectNodeTypeSelector;
template <> struct ProjectNodeTypeSelector<Track> { static const ProjectNodeType type = ProjectNodeType::Track; };
template <> struct ProjectNodeTypeSelector<MapObjectContainer> { static const ProjectNodeType type = ProjectNodeType::Database; };

class ProjectTreeNodeBase : public ww::TreeNode
{
public:
	ProjectTreeNodeBase(std::string text, ProjectNodeType type) : ww::TreeNode(text), mType(type) {}
	ProjectNodeType GetType() const { return mType; }

protected:
	ProjectNodeType mType;
};

template <class T>
class ProjectTreeNode : public ProjectTreeNodeBase
{
public:
	ProjectTreeNode(std::string text, const T* object) : ProjectTreeNodeBase(text, ProjectNodeTypeSelector<T>::type), mObject(object) {}

	const T* GetObject() const { return mObject; }

private:
	const T* mObject;
};

class Project;
class WindowMapRenderer;

class ProjectManager
{
public:
	ProjectManager(mt::WindowMapRenderer* renderer, ww::TreeView* projectTree);
	~ProjectManager();

	bool LoadProject(std::string filename);
	void UnloadProject();

private:
	void OnSelChanged(ww::TreeNode* newNode, ww::TreeNode* oldNode);

private:
	Project* mProject;
	mt::WindowMapRenderer* mRenderer;
	ww::TreeView* mProjectTree;
};

}

#endif // !__MULTITRACKS_PROJECTMANAGER_H__