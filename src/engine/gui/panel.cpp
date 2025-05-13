#include "panel.hpp"

Panel::Panel()
{
	m_IsVisible = true;
}

Panel::~Panel()
{
	m_IsVisible = false;
}