#pragma once

#include "mc/deps/game_refs/GameRefs.h"
#include <memory>

template <>
class GameRefs<EntityContext> {
public:
    using OwnerStorage       = std::shared_ptr<EntityContext>;
    using StackResultStorage = std::shared_ptr<EntityContext>;
    using WeakStorage        = std::weak_ptr<EntityContext>;
    using StackRef           = EntityContext;
};