#include "GameObject.h"



GameObject::GameObject()
{
}


GameObject::~GameObject()
{
	delete[] m_Object;
}

void GameObject::update()
{
	 
}

void GameObject::draw(unsigned int ProgramId)
{
	m_Object->draw(ProgramId);
}
