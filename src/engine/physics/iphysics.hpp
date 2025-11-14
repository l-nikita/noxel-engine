#pragma once

class IPhysics
{
public:	
	virtual ~IPhysics() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;

	virtual bool IsRunning() = 0;

	virtual void FlushPendingDestructions() = 0;
};

extern IPhysics* g_Physics;