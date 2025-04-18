#include <iostream>
#include <pthread.h>
#include <string>

int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // can use join
    int detach_state;
    pthread_attr_getdetachstate(&attr, &detach_state);
    std::cout << "Detach state: " << (detach_state == PTHREAD_CREATE_JOINABLE ? "JOINABLE" : "DETACHED") 
        << std::endl;

    // concurrency scope
    int scope;
    pthread_attr_getscope(&attr, &scope);
    std::cout << "Scope: " << (scope == PTHREAD_SCOPE_SYSTEM ? "SYSTEM" : "PROCESS") 
        << std::endl;

    // zero values for auto setup
    void* stack_addr;
    size_t stack_size;
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    std::cout << "Stack address: " << stack_addr << std::endl;
    std::cout << "Stack size: " << stack_size << " bytes" << std::endl;

    // inherit scheduling policy and priority from parent
    int inherit_sched;
    pthread_attr_getinheritsched(&attr, &inherit_sched);
    std::cout << "Inherit scheduler: " << (inherit_sched == PTHREAD_INHERIT_SCHED ? "INHERIT" : "EXPLICIT")
        << std::endl;

    int policy;
    pthread_attr_getschedpolicy(&attr, &policy);
    std::string policy_str;
    switch(policy)
    {
        case SCHED_OTHER:
            policy_str = "SCHED_OTHER"; // default
            break;
        case SCHED_FIFO:
            policy_str = "SCHED_FIFO";
            break;
        case SCHED_RR:
            policy_str = "SCHED_RR"; // round-robin
            break;
        default:
            policy_str = "UNKNOWN";
            break;
    }
    std::cout << "Scheduling policy: " << policy_str << std::endl;

    // one page by default
    size_t guard_size; 
    pthread_attr_getguardsize(&attr, &guard_size);
    std::cout << "Stack guard zize: " << guard_size << std::endl;

    // priority for FIFO or RR
    sched_param param;
    pthread_attr_getschedparam(&attr, &param);
    std::cout << "Sched priority: " << param.sched_priority << std::endl;

    pthread_attr_destroy(&attr);
    return 0;
}
