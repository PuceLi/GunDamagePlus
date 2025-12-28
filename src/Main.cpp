#include "Main.h"

#include "ll/api/mod/RegisterHelper.h"

#include "features/OwnerFix.h"
#include "features/NoHurtTime.h"

namespace main {

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
        OwnerFix::enable();
        NoHurtTime::enable();
    
        return true;
    }

    bool Main::disable() {
        getSelf().getLogger().info("GunEnhance Disabling...");
    
        OwnerFix::disable();
        NoHurtTime::disable();
    
        return true;
    }

} // namespace main

LL_REGISTER_MOD(main::Main, main::Main::getInstance());