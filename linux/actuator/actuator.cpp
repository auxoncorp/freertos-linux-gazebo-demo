#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdlib.h>
#include <assert.h>

#include <gz/transport/Node.hh>
#include <gz/msgs/twist.pb.h>

#include "actuator/actuator.h"

struct actuator
{
    void* node; // gz::transport::Node
    void* pub;  // gz::transport::Node::Publisher
    void* msg; // gz::msgs::Twist
    uint32_t seqnum;
};

actuator_s* actuator_new()
{
    actuator_s* act;
    act = (actuator_s*) malloc(sizeof(*act));
    assert(act != NULL);
    act->seqnum = 0;

    gz::transport::Node* node = new gz::transport::Node();
    act->node = node;

    auto topic = "/cmd_vel";
    gz::transport::Node::Publisher* pub = new gz::transport::Node::Publisher();
    *pub = node->Advertise<gz::msgs::Twist>(topic);
    act->pub = pub;

    gz::msgs::Twist* msg = new gz::msgs::Twist();
    msg->Clear();
    assert(msg->mutable_linear() != NULL);

    assert(msg->mutable_header() != NULL);
    auto seqnum_data = msg->mutable_header()->add_data();
    assert(seqnum_data != NULL);
    seqnum_data->set_key("seq");

    act->msg = msg;
    act->seqnum = 1;

    return act;
}

void actuator_free(actuator_s* act)
{
    if(act != NULL)
    {
        assert(act->msg != NULL);
        delete static_cast<gz::msgs::Twist*>(act->msg);

        assert(act->pub != NULL);
        delete static_cast<gz::transport::Node::Publisher*>(act->pub);

        assert(act->node != NULL);
        delete static_cast<gz::transport::Node*>(act->node);

        free(act);
    }
}

uint32_t actuator_seqnum(actuator_s* act)
{
    assert(act != NULL);
    return act->seqnum;
}

void actuator_set_vel(actuator_s* act, double vel)
{
    gz::msgs::Twist* msg;
    gz::transport::Node::Publisher* pub;

    assert(act != NULL);
    assert(act->msg != NULL);
    assert(act->pub != NULL);

    msg = static_cast<gz::msgs::Twist*>(act->msg);
    msg->mutable_linear()->set_x(vel);

    std::string sn = std::to_string(act->seqnum);
    msg->mutable_header()->mutable_data(0)->clear_value();
    msg->mutable_header()->mutable_data(0)->add_value(sn);

    pub = static_cast<gz::transport::Node::Publisher*>(act->pub);
    pub->Publish(*msg);

    act->seqnum += 1;
}
