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
#include <gz/msgs/contacts.pb.h>

#include "frozen.h"

#include "ImuRelay.hh"

#define NS_PER_SEC (1000000000ULL)

GZ_ADD_PLUGIN(
    emulation_support::ImuRelay,
    gz::sim::System,
    emulation_support::ImuRelay::ISystemConfigure)

#define BUF_SIZE (1024)

using namespace emulation_support;

class emulation_support::ImuRelayPrivate
{
    public:
        gz::transport::Node node;
        int sock;
        struct sockaddr_in servaddr;
        bool connected;
        bool bump_detected;
        char json_buf[BUF_SIZE];
};

ImuRelay::ImuRelay(): System(), data_ptr(std::make_unique<ImuRelayPrivate>())
{
    this->data_ptr->sock = -1;
    this->data_ptr->connected = false;
    this->data_ptr->bump_detected = false;
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

    gzlog << "ImuRelay connecting to " << addr << ":" << port << std::endl;

    auto contact_msg_cb = std::function<void(const gz::msgs::Contacts &)>(
        [this](const gz::msgs::Contacts &msg)
        {
            if(!this->data_ptr->bump_detected)
            {
                gzlog << "CONTACT" << std::endl;
                this->data_ptr->bump_detected = true;
            }
        });

    auto msg_cb = std::function<void(const gz::msgs::IMU &)>(
        [this](const gz::msgs::IMU &msg)
        {
            if(!this->data_ptr->connected)
            {
                if(connect(this->data_ptr->sock, (struct sockaddr*) &this->data_ptr->servaddr, sizeof(this->data_ptr->servaddr)) != 0)
                {
                    return;
                }

                gzlog << "Connected to IMU relay server" << std::endl;

                this->data_ptr->connected = true;
            }

            if(this->data_ptr->connected)
            {
                auto timestamp = msg.header().stamp();
                uint64_t ts_ns = ((uint64_t) timestamp.sec()) * NS_PER_SEC;
                ts_ns += (uint64_t) timestamp.nsec();

                std::string native_seqnum = msg.header().data(1).value(0);

                //gzdbg << "seqnum: " << native_seqnum << ", bump: " << this->data_ptr->bump_detected << std::endl;

                struct json_out out = JSON_OUT_BUF(this->data_ptr->json_buf, BUF_SIZE);
                int size = json_printf(
                        &out,
                        "{sim_time: %llu, seqnum: %s, bump: %d, wheel_speed: %lf}",
                        ts_ns,
                        native_seqnum.c_str(),
                        this->data_ptr->bump_detected ? 1 : 0,
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

    // TODO - cfg for this
    auto contact_topic_name = gz::transport::TopicUtils::AsValidTopic("/world/Moving_robot/model/vehicle_blue/link/chassis/sensor/chassis_contact/contact");
    if(!this->data_ptr->node.Subscribe(contact_topic_name, contact_msg_cb))
    {
        gzerr << "Subscriber could not be created for topic ["
            << contact_topic_name << "] with message type [Contacts]" << std::endl;
        return;
    }
}
