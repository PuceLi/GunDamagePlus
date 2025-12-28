#include "OwnerFix.h"

#include "ll/api/memory/hook.h"

#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/components/ShooterItemComponent.h"
#include "mc/deps/core/math/Vec3.h"

using namespace ll::memory;

namespace OwnerFix {

    thread_local Actor* g_ScriptShooter = nullptr;

    LL_AUTO_TYPE_INSTANCE_HOOK(
        ActorGetHeadPosHook,
        HookPriority::Normal,
        Actor,
        &Actor::getHeadPos,
        Vec3
    ) {
        if (this->isPlayer()) {
            g_ScriptShooter = this;
        }
        return origin();
    }

    LL_AUTO_TYPE_INSTANCE_HOOK(
        ShooterUseHook,
        HookPriority::Normal,
        ShooterItemComponent,
        ll::memory::unchecked(&ShooterItemComponent::use),
        void,
        bool& result,
        ItemStack& item,
        Player& player
    ) {
        g_ScriptShooter = &player;
        origin(result, item, player);
        g_ScriptShooter = nullptr;
    }

    LL_AUTO_TYPE_INSTANCE_HOOK(
        ShooterReleaseHook,
        HookPriority::Normal,
        ShooterItemComponent,
        ll::memory::unchecked(&ShooterItemComponent::releaseUsing),
        bool,
        ItemStack& item,
        Player* player,
        int duration
    ) {
        g_ScriptShooter = player;
        bool result = origin(item, player, duration);
        g_ScriptShooter = nullptr;
        return result;
    }

    LL_AUTO_TYPE_INSTANCE_HOOK(
        ShooterProjectilesHook,
        HookPriority::Normal,
        ShooterItemComponent,
        ll::memory::unchecked(&ShooterItemComponent::_shootProjectiles),
        void,
        ItemStack& item,
        Player* player,
        int duration
    ) {
        g_ScriptShooter = player;
        origin(item, player, duration);
        g_ScriptShooter = nullptr;
    }

    LL_AUTO_TYPE_INSTANCE_HOOK(
        ActorInitHook,
        HookPriority::Normal,
        Actor,
        &Actor::$initializeComponents,
        void,
        ActorInitializationMethod method,
        VariantParameterList const& params
    ) {
        origin(method, params);

        if (g_ScriptShooter && this != g_ScriptShooter) {
            this->setOwner(g_ScriptShooter->getOrCreateUniqueID());
        }
    }

    bool enable() {
        ActorGetHeadPosHook::hook();
        ShooterUseHook::hook();
        ShooterReleaseHook::hook();
        ShooterProjectilesHook::hook();
        ActorInitHook::hook();
        return true;
    }

    bool disable() {
        ActorGetHeadPosHook::unhook();
        ShooterUseHook::unhook();
        ShooterReleaseHook::unhook();
        ShooterProjectilesHook::unhook();
        ActorInitHook::unhook();
        return true;
    }
}