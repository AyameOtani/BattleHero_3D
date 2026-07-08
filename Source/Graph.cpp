#include "Graph.h"

Graph::Graph(std::string filename, VECTOR initPos, float Rate)
	:Object2D(filename,initPos)
{

}

Graph::~Graph()
{

}


void Graph::Update()
{
	Object2D::Update();
}

void Graph::Draw()
{
	Object2D::Draw();
}

int Graph::PlayerPhoto(std::string filename)
{
	static int photoHandle;

	if (filename == "Resource/3d/Player/Hero.mv1")
	{
		photoHandle = LoadGraph("Resource/Graph/Hero.png");
	}
	else if (filename == "Resource/3d/Zombie/Zombie_Mesh.mv1")
	{
		photoHandle = LoadGraph("Resource/Graph/Zombi.png");
	}
	else if (filename == "Resource/3d/Military/Guntai.mv1")
	{
		photoHandle = LoadGraph("Resource/Graph/guntai.png");
	}
	else if (filename == "Resource/3d/Bob/Bob.mv1")
	{
		photoHandle = LoadGraph("Resource/Graph/Bob.png");
	}
	else if (filename == "Resource/3d/Devil/Devil.mv1")
	{
		photoHandle = LoadGraph("Resource/Graph/Devil.png");
	}

	return photoHandle;
}

// ïâÇØÇΩéûÇÃâÊëú
int Graph::DefeatPhoto(std::string filename)
{
	static int defeatHandle;

	if (filename == "Resource/3d/Player/Hero.mv1")
	{
		defeatHandle = LoadGraph("Resource/defeat/Hero.png");
	}
	else if (filename == "Resource/3d/Zombie/Zombie_Mesh.mv1")
	{
		defeatHandle = LoadGraph("Resource/defeat/Zombi.png");
	}
	else if (filename == "Resource/3d/Military/Guntai.mv1")
	{
		defeatHandle = LoadGraph("Resource/defeat/Guntai.png");
	}
	else if (filename == "Resource/3d/Bob/Bob.mv1")
	{
		defeatHandle = LoadGraph("Resource/defeat/Bob.png");
	}
	else if (filename == "Resource/3d/Devil/Devil.mv1")
	{
		defeatHandle = LoadGraph("Resource/defeat/Niku.png");
	}
	else
	{
		defeatHandle = LoadGraph("Resource/defeat/Hero.png");
	}

	return defeatHandle;
}




// èüÇ¡ÇΩéûÇÃâÊëú
int Graph::VictoryPhoto(std::string filename)
{
	static int victoryHandle;

	if (filename == "Resource/3d/Player/Hero.mv1")
	{
		victoryHandle = LoadGraph("Resource/victory/Hero.png");
	}
	else if (filename == "Resource/3d/Zombie/Zombie_Mesh.mv1")
	{
		victoryHandle = LoadGraph("Resource/victory/Zombi.png");
	}
	else if (filename == "Resource/3d/Military/Guntai.mv1")
	{
		victoryHandle = LoadGraph("Resource/victory/Guntai.png");
	}
	else if (filename == "Resource/3d/Bob/Bob.mv1")
	{
		victoryHandle = LoadGraph("Resource/victory/Bob.png");
	}
	else if (filename == "Resource/3d/Devil/Devil.mv1")
	{
		victoryHandle = LoadGraph("Resource/victory/Niku.png");
	}
	else
	{
		victoryHandle = LoadGraph("Resource/victory/Hero.png");
	}

	return victoryHandle;
}

