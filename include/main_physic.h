// NOT A PROJECT TO BE TAKEN SERIOUSLY
// Physic: PhysX (https://developer.nvidia.com/physx-sdk)

#pragma once

#include "physx/include/PxPhysicsAPI.h"
#include "physx/include/pvd/PxPvd.h"

namespace context {
    struct ContextPhysic {
        ContextPhysic() {
            foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, callback);
            pvd = physx::PxCreatePvd(*foundation);
            transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        }

        ~ContextPhysic() = default;

        physx::PxFoundation* foundation{nullptr};
        physx::PxPvd* pvd{};
        physx::PxPvdTransport* transport{};
        static inline physx::PxDefaultAllocator allocator{};
        static inline physx::PxDefaultErrorCallback callback{};
    };
}
