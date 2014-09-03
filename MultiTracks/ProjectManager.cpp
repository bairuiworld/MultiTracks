#include "FileDialog.h"
#include "Project.h"
#include "Track.h"
#include "GPX.h"
#include "ProjectManager.h"

#include "MapRenderer.h"

namespace mt
{

ProjectManager::ProjectManager(mt::WindowMapRenderer* renderer, ww::TreeView* projectTree) :
mProject(nullptr), mRenderer(renderer), mProjectTree(projectTree)
{
	mProjectTree->SignalSelChanged += sig::slot(this, &ProjectManager::OnSelChanged);
	mProjectTree->SignalItemClick += sig::slot(this, &ProjectManager::OnTreeClick);
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

	LoadProjectTree();
	return true;
}

void ProjectManager::LoadProjectTree()
{
	ww::TreeNode* projectNode = new ProjectTreeNodeBase(mProject->GetName(), ProjectNodeType::Project);
	projectNode->AddNode(new ProjectTreeNode<MapObjectContainer>("Base de sections", &mProject->GetDatabase()));

	for(std::string group : mProject->GetGroups())
	{
		ww::TreeNode* groupNode = new ProjectTreeNodeBase(group, ProjectNodeType::Group);
		for(Track* track : mProject->GetTracks(group))
			groupNode->AddNode(new ProjectTreeNode<Track>(track->GetName(), track));
		projectNode->AddNode(groupNode);
	}
	mProjectTree->AddNode(projectNode);
}

void ProjectManager::UnloadProject()
{
	mProjectTree->RemoveAllNodes();
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

void ProjectManager::OnTreeClick(ww::TreeNode* node_, ww::MouseEvent ev)
{
	if(ev.GetButton() != ww::MouseButton::Right) return;
	if(ev.GetClicks() != 1) return;
	ProjectTreeNodeBase* node = reinterpret_cast<ProjectTreeNodeBase*>(node_);
	if(node->GetType() == ProjectNodeType::Group)
	{
		ww::PopupMenu menu;
		menu.AddItem("Importer une trace...", std::bind(&ProjectManager::ImportTrack, this, node));
		menu.Track(mProjectTree->GetHandle(), ev.GetPoint());
	}
}

void ProjectManager::ImportTrack(ProjectTreeNodeBase* groupNode)
{
	ww::FileDialog opendialog("Trace GPS (*.gpx)\0*.gpx\0", GetParent(mProjectTree->GetHandle()));
	if(opendialog.Open())
	{
		Track* track = GPX::Load(opendialog.GetFileName().c_str());
		if(track)
		{
			mProject->AddTrack(track, groupNode->GetText());
			ww::TreeNode* trackNode = new ProjectTreeNode<Track>(track->GetName(), track);
			groupNode->AddNode(trackNode);
			trackNode->Select();
			trackNode->EnsureVisible();
		}
	}
}

}