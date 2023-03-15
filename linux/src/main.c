#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <actuator/actuator.h>

#include "wire_protocol.h"
#include "app_types.h"
#define TRACEPOINT_DEFINE
#include "tp.h"

static volatile sig_atomic_t g_exit_signaled = 0;

static void sigint_handler(int sig)
{
    (void) sig;
    g_exit_signaled = 1;
}

static void sigint_register(void)
{
    int err;
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_RESTART;
    act.sa_handler = sigint_handler;
    err = sigaction(SIGINT, &act, 0);
    assert(err == 0);
    err = sigaction(SIGTERM, &act, 0);
    assert(err == 0);
}

static uint16_t clamped_duty(uint16_t duty)
{
    if(duty > 1000)
    {
        duty = 1000;
    }
    return duty;
}

int main(int argc, char** argv)
{
    int err;
    uint16_t duty;
    int msg_socket;
    wire_msg_s msg;
    fd_set fds;
    struct timeval tv = {0};
    struct sockaddr_in addr = {0};
    cpu_set_t mask;
    actuator_s* actuator;
    (void) argc;
    (void) argv;

    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    err = sched_setaffinity(0, sizeof(mask), &mask);
    assert(err == 0);

    tracepoint(actuator, startup);

    sigint_register();

    msg_socket = socket(AF_INET, SOCK_DGRAM, 0);
    assert(msg_socket != -1);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SENSOR_DATA_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(msg_socket, (const struct sockaddr*) &addr, sizeof(addr));
    assert(err == 0);

    actuator = actuator_new();

    tracepoint(actuator, init);

    duty = 0;
    tracepoint(actuator, set_pwm, duty);
    actuator_set_vel(actuator, 0.0);

    while(g_exit_signaled == 0)
    {
        FD_ZERO(&fds);
        FD_SET(msg_socket, &fds);

        const int status = select(msg_socket + 1, &fds, NULL, NULL, &tv);
        assert((status >= 0) || (errno == EINTR));

        if(status > 0)
        {
            const ssize_t msg_size = recv(msg_socket, &msg, sizeof(msg), 0);
            assert((msg_size == WIRE_MSG_SIZE) || (errno == EAGAIN));

            if(msg_size == WIRE_MSG_SIZE)
            {
                tracepoint(
                        actuator,
                        received_msg,
                        msg.magic0,
                        msg.magic1,
                        msg.type,
                        msg.seqnum,
                        msg.wheel_speed,
                        msg.seqnum, /* remote_nonce */
                        &msg.tid[0] /* remote_timeline_id */);

                if(msg.magic0 != WIRE_MAGIC0)
                {
                    tracepoint(actuator, invalid_msg, BAD_MAGIC0);
                }
                else if(msg.magic1 != WIRE_MAGIC1)
                {
                    tracepoint(actuator, invalid_msg, BAD_MAGIC1);
                }
                else if(msg.type != WIRE_TYPE_WHEEL_SPEED)
                {
                    tracepoint(actuator, invalid_msg, BAD_TYPE);
                }
                else
                {
                    /* Handle valid message */
                    duty = clamped_duty(duty + 10);
                    double target_velocity = ((double) duty / 1000.0) * 1.0;
                    tracepoint(actuator, target_velocity, target_velocity);
                    tracepoint(actuator, set_pwm, duty);
                    actuator_set_vel(actuator, target_velocity);
                }
            }
        }
    }

    tracepoint(actuator, target_velocity, 0.0);
    actuator_set_vel(actuator, 0.0);

    close(msg_socket);

    actuator_free(actuator);

    tracepoint(actuator, deinit);

    return EXIT_SUCCESS;
}
