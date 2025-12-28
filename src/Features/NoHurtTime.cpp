#include "NoHurtTime.h"

#include "ll/api/memory/hook.h"

#include "mc/world/actor/Actor.h"
#include "mc/world/actor/ActorDamageSource.h"
#include "mc/deps/shared_types/legacy/actor/ActorDamageCause.h"

using namespace ll::memory;

namespace NoHurtTime {

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

    bool enable() {
        ActorHurtHook::hook();
        return true;
    }

    bool disable() {
        ActorHurtHook::unhook();
        return true;
    }
}