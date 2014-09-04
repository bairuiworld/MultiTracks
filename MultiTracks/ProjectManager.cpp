#include "FileDialog.h"
#include "Project.h"
#include "Track.h"
#include "GPX.h"
#include "ProjectManager.h"

#include "MapRenderer.h"
#include "Map.h"
#include "MapViewport.h"

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
		Track* track = reinterpret_cast<ProjectTreeNode<Track>*>(node)->GetObject();
		SignalSelectTrack.emit(track);
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
	else if(node->GetType() == ProjectNodeType::Track)
	{
		ww::PopupMenu menu;
		Track* track = reinterpret_cast<ProjectTreeNode<Track>*>(node_)->GetObject();
		menu.AddItem("Expoter sur une carte...", std::bind(&ProjectManager::ExportTrackOnMap, this, track));
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
			mProject->Save();
			ww::TreeNode* trackNode = new ProjectTreeNode<Track>(track->GetName(), track);
			groupNode->AddNode(trackNode);
			trackNode->Select();
			SignalSelectTrack.emit(track);
			trackNode->EnsureVisible();
		}
	}
}

void ProjectManager::ExportTrackOnMap(Track* track)
{
	MapViewport* vp = mRenderer->GetMap()->GetViewport();
	MapViewport save(*vp);
	Area area = track->GetBoundingBox();
	vp->SetView(area);
	mRenderer->Save(L"a.jpg", ImageFormat::jpeg);
	vp->SetOrigin(save.GetOrigin());
	vp->SetViewDimension(save.GetWidth(), save.GetHeight());
}

}