#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

// WiFi application task
#define WIFI_APP_TASK_STACK_SIZE                4096
#define WIFI_APP_TASK_PRIORITY                  5
#define WIFI_APP_TASK_CORE_ID                   0

// HTTP Server task
#define HTTP_SERVER_TASK_STACK_SIZE             8192
#define HTTP_SERVER_TASK_PRIORITY               4
#define HTTP_SERVER_TASK_CORE_ID                0

// HTTP Server PingHost Task
#define HTTP_SERVER_PINGHOST_TASK_STACK_SIZE    4096
#define HTTP_SERVER_PINGHOST_TASK_PRIORITY      2
#define HTTP_SERVER_PINGHOST_TASK_CORE_ID       0

// Animation task
#define ANIMATION_TASK_STACK_SIZE               8192
#define ANIMATION_TASK_PRIORITY                 3
#define ANIMATION_TASK_CORE_ID                  0

// Matrix Control task
#define MATRIX_CONTROL_TASK_STACK_SIZE          8192
#define MATRIX_CONTROL_TASK_PRIORITY            3
#define MATRIX_CONTROL_TASK_CORE_ID             0

// Matrix Child task
#define MATRIX_CHILD_TASK_STACK_SIZE            4096
#define MATRIX_CHILD_TASK_PRIORITY              3
#define MATRIX_CHILD_TASK_CORE_ID               0

// UDP Server task (RemoteEffect)
#define UDP_SERVER_TASK_STACK_SIZE              4096
#define UDP_SERVER_CHILD_TASK_PRIORITY          4
#define UDP_SERVER_CHILD_TASK_CORE_ID           0

// UDP Client task (UdpMatrix)
#define UDP_CLIENT_TASK_STACK_SIZE              4096
#define UDP_CLIENT_TASK_PRIORITY                4
#define UDP_CLIENT_TASK_CORE_ID                 0

#endif