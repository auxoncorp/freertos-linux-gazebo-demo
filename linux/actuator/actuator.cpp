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
};

actuator_s* actuator_new()
{
    actuator_s* act;
    act = (actuator_s*) malloc(sizeof(*act));
    assert(act != NULL);

    gz::transport::Node* node = new gz::transport::Node();
    act->node = node;

    auto topic = "/cmd_vel";
    gz::transport::Node::Publisher* pub = new gz::transport::Node::Publisher();
    *pub = node->Advertise<gz::msgs::Twist>(topic);
    act->pub = pub;

    gz::msgs::Twist* msg = new gz::msgs::Twist();
    assert(msg->mutable_linear() != NULL);
    act->msg = msg;

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

void actuator_set_vel(actuator_s* act, double vel)
{
    gz::msgs::Twist* msg;
    gz::transport::Node::Publisher* pub;

    assert(act != NULL);
    assert(act->msg != NULL);
    assert(act->pub != NULL);

    msg = static_cast<gz::msgs::Twist*>(act->msg);
    msg->Clear();
    msg->mutable_linear()->set_x(vel);

    pub = static_cast<gz::transport::Node::Publisher*>(act->pub);
    pub->Publish(*msg);
}
