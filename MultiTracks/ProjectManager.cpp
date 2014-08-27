#include "Project.h"
#include "Track.h"
#include "ProjectManager.h"

#include "MapRenderer.h"

namespace mt
{

ProjectManager::ProjectManager(mt::WindowMapRenderer* renderer, ww::TreeView* projectTree) :
mProject(nullptr), mRenderer(renderer), mProjectTree(projectTree)
{
	mProjectTree->SignalSelChanged += sig::slot(this, &ProjectManager::OnSelChanged);
}

ProjectManager::~ProjectManager()
{
	delete mProject;
}

bool ProjectManager::LoadProject(std::string filename)
{
	if(mProject) UnloadProject();
	mProject = new Project;
	if(!mProject->LoadXML(filename))
	{
		delete mProject;
		mProject = nullptr;
		return false;
	}

	ww::TreeNode* projectNode = new ProjectTreeNodeBase(mProject->GetName(), ProjectNodeType::Project);
	projectNode->AddNode(new ProjectTreeNode<MapObjectContainer>("Base de sections", &mProject->GetDatabase()));

	for(std::string group : mProject->GetGroups())
	{
		ww::TreeNode* groupNode = new ProjectTreeNodeBase(group.c_str(), ProjectNodeType::Group);
		for(Track* track : mProject->GetTracks(group))
			groupNode->AddNode(new ProjectTreeNode<Track>(track->GetName(), track));
		projectNode->AddNode(groupNode);
	}
	mProjectTree->AddNode(projectNode);
	return true;
}

void ProjectManager::UnloadProject()
{
	// clear tree
	delete mProject;
	mProject = nullptr;
}

void ProjectManager::OnSelChanged(ww::TreeNode* newNode, ww::TreeNode* oldNode)
{
	ProjectTreeNodeBase* node = reinterpret_cast<ProjectTreeNodeBase*>(newNode);
	switch(node->GetType())
	{
	case ProjectNodeType::Track:
	{
		const Track* track = reinterpret_cast<ProjectTreeNode<Track>*>(node)->GetObject();
		mRenderer->AddComponent(track);
	} break;

	case ProjectNodeType::Database:
	{
		const MapObjectContainer* container = reinterpret_cast<ProjectTreeNode<MapObjectContainer>*>(node)->GetObject();
		mRenderer->AddComponent(container);
	} break;
	}
}

}