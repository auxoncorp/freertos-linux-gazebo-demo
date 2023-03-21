#include <cstdlib>

#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <gz/plugin/Register.hh>
#include <gz/sim/Util.hh>
#include <gz/transport/Node.hh>
#include <gz/msgs/imu.pb.h>

#include "frozen.h"

#include "ImuRelay.hh"

#define NS_PER_SEC (1000000000ULL)

GZ_ADD_PLUGIN(
    emulation_support::ImuRelay,
    gz::sim::System,
    emulation_support::ImuRelay::ISystemConfigure)

#define BUF_SIZE (512)

using namespace emulation_support;

class emulation_support::ImuRelayPrivate
{
    public:
        gz::transport::Node node;
        int sock;
        struct sockaddr_in servaddr;
        bool connected;
        uint32_t seqnum;
        char json_buf[BUF_SIZE];
};

ImuRelay::ImuRelay(): System(), data_ptr(std::make_unique<ImuRelayPrivate>())
{
    this->data_ptr->sock = -1;
    this->data_ptr->connected = false;
    memset(&this->data_ptr->servaddr, 0, sizeof(this->data_ptr->servaddr));
}

ImuRelay::~ImuRelay()
{
    if(this->data_ptr->sock != -1)
    {
        close(this->data_ptr->sock);
        this->data_ptr->sock = -1;
    }
    this->data_ptr->connected = false;
}

void ImuRelay::Configure(
        const gz::sim::Entity & entity,
        const std::shared_ptr<const sdf::Element> &sdf,
        gz::sim::EntityComponentManager & ecm,
        gz::sim::EventManager &)
{
    auto topic_name = gz::transport::TopicUtils::AsValidTopic(sdf->Get<std::string>("topic_name"));
    if(topic_name.empty())
    {
        gzerr << "Invalid input topic [" << topic_name << "]" << std::endl;
        return;
    }

    auto addr = sdf->Get<std::string>("address");
    if(addr.empty())
    {
        gzerr << "Invalid address" << std::endl;
        return;
    }

    auto port = sdf->Get<int>("port");

    this->data_ptr->sock = socket(AF_INET, SOCK_STREAM, 0);
    this->data_ptr->servaddr.sin_family = AF_INET;
    this->data_ptr->servaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    this->data_ptr->servaddr.sin_port = htons(port);
    this->data_ptr->seqnum = 0;

    gzlog << "ImuRelay connecting to " << addr << ":" << port << std::endl;

    auto msg_cb = std::function<void(const gz::msgs::IMU &)>(
        [this](const gz::msgs::IMU &msg)
        {
            if(!this->data_ptr->connected)
            {
                if(connect(this->data_ptr->sock, (struct sockaddr*) &this->data_ptr->servaddr, sizeof(this->data_ptr->servaddr)) != 0)
                {
                    gzerr << "Failed to connect to IMU relay server" << std::endl;
                    return;
                }

                this->data_ptr->connected = true;
            }

            if(this->data_ptr->connected)
            {
                auto timestamp = msg.header().stamp();
                uint64_t ts_ns = ((uint64_t) timestamp.sec()) * NS_PER_SEC;
                ts_ns += (uint64_t) timestamp.nsec();

                this->data_ptr->seqnum += 1;
                std::string native_seqnum = msg.header().data(1).value(0);

                struct json_out out = JSON_OUT_BUF(this->data_ptr->json_buf, 512);
                int size = json_printf(
                        &out,
                        "{sim_time: %llu, seqnum: %lu, native_seqnum: %s, wheel_speed: %lf}",
                        ts_ns,
                        this->data_ptr->seqnum,
                        native_seqnum.c_str(),
                        msg.angular_velocity().z());
                if(size <= 0)
                {
                    gzerr << "Failed to encode message" << std::endl;
                }

                int err = write(this->data_ptr->sock, this->data_ptr->json_buf, size);
                if(err == -1)
                {
                    gzerr << "Failed to write to socket" << std::endl;
                }
            }
        });

    if(!this->data_ptr->node.Subscribe(topic_name, msg_cb))
    {
        gzerr << "Subscriber could not be created for topic ["
            << topic_name << "] with message type [IMU]" << std::endl;
        return;
    }
}
