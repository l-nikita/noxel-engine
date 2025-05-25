#include <entt/entt.hpp>
#include "singleton.hpp"

class Entities : public Singleton<Entities>
{
public:
	entt::registry m_Registry;
};