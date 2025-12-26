#include "Main.h"


#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/memory/hook.h"


#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/item/components/ShooterItemComponent.h"
#include "mc/world/actor/ActorDamageSource.h"
#include "mc/deps/shared_types/legacy/actor/ActorDamageCause.h"

#include "mc/world/actor/ActorInitializationMethod.h"
#include "mc/util/VariantParameterList.h"

using namespace ll::memory;

namespace main {

thread_local Actor* g_CurrentShooter = nullptr;

LL_AUTO_TYPE_INSTANCE_HOOK(
    ShooterReleaseHook,
    HookPriority::Normal,
    ShooterItemComponent,
    &ShooterItemComponent::releaseUsing,
    bool,
    ItemStack& item,
    Player* player,
    int duration
) {
    g_CurrentShooter = player;

    bool result = origin(item, player, duration);

    g_CurrentShooter = nullptr;

    return result;
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

    if (g_CurrentShooter) {
        
        if (this != g_CurrentShooter) {

            this->setOwner(g_CurrentShooter->getOrCreateUniqueID());
        }
    }
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    ActorHurtHook,
    HookPriority::Normal,
    Actor,
    &Actor::$_hurt,
    bool,
    ActorDamageSource const& source,
    float damage,
    bool knock,
    bool ignite
) {
    auto const* causePtr = reinterpret_cast<SharedTypes::Legacy::ActorDamageCause const*>(&source.mCause);
    
    if (causePtr && *causePtr == SharedTypes::Legacy::ActorDamageCause::Projectile) {

        auto* invulnerableTimePtr = reinterpret_cast<int*>(&this->mInvulnerableTime);
        if (invulnerableTimePtr) {
            *invulnerableTimePtr = 0;
        }
    }

    return origin(source, damage, knock, ignite);
}

Main& Main::getInstance() {
    static Main instance;
    return instance;
}

bool Main::load() {
    getSelf().getLogger().info("GunEnhance Loading...");
    getSelf().getLogger().info("Author: PuceLi");
    return true;
}

bool Main::enable() {
    getSelf().getLogger().info("GunEnhance Enabling...");
    
    ShooterReleaseHook::hook();
    ActorInitHook::hook();
    ActorHurtHook::hook();
    
    return true;
}

bool Main::disable() {
    getSelf().getLogger().info("GunEnhance Disabling...");
    
    ShooterReleaseHook::unhook();
    ActorInitHook::unhook();
    ActorHurtHook::unhook();
    
    return true;
}

} // namespace main

LL_REGISTER_MOD(main::Main, main::Main::getInstance());