#ifndef H_SCHEDULER_H
#define H_SCHEDULER_H

#include <stdlib.h>
#include <mmp/mmp_error.h>
#include <mmp/mmp_socket.h>

typedef int t_sched_id;

t_sched_id scheduler_create(size_t pool_size);
void scheduler_destroy(t_sched_id sched_id);
ret_t scheduler_add_listen_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_add_client_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_del_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_loop(t_sched_id sched_id, void(*cback_fp)(t_socket));

#endif /* H_SCHEDULER_H */

