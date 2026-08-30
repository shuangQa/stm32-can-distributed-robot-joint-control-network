//
// Created by 双 on 2026/8/28.
//

#ifndef JOINTNODE_STATE_MACHINE_H
#define JOINTNODE_STATE_MACHINE_H

/*节点状态机的状态*/
typedef enum
{
    NODE_INIT,
    NODE_REGISTERING,
    NODE_ONLINE,
    NODE_FAULT,
    NODE_OFFLINE,
}NodeState_t;

extern NodeState_t g_state;

/*设置状态函数，单独封装便于调试*/
void StateMachine_SetState(NodeState_t state);

/*节点状态机函数*/
void StateMachine_Update();

#endif //JOINTNODE_STATE_MACHINE_H
