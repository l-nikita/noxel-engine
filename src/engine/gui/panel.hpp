#pragma once

class Panel
{
public:
	Panel();
	~Panel();

	bool m_IsVisible;
	virtual void Draw() {};
};