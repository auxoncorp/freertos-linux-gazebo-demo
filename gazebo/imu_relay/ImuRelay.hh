#ifndef IMU_SERVER_PLUGIN_HH
#define IMU_SERVER_PLUGIN_HH

#include <memory>
#include <gz/sim/System.hh>

namespace emulation_support
{
    class ImuRelayPrivate;

    class ImuRelay:
        public gz::sim::System,
        public gz::sim::ISystemConfigure
        {
            public: ImuRelay();

            public: ~ImuRelay() override;

            public: void Configure(
                            const gz::sim::Entity &entity,
                            const std::shared_ptr<const sdf::Element> &sdf,
                            gz::sim::EntityComponentManager & ecm,
                            gz::sim::EventManager & event_mngr) override;

            private: std::unique_ptr<ImuRelayPrivate> data_ptr;
        };
}

#endif /* IMU_SERVER_PLUGIN_HH */
