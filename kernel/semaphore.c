/*
 * Copyright (C) 2015  Raphaël Poggi <poggi.raph@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <signal.h>

#include <semaphore.h>
#include <task.h>
#include <scheduler.h>

static void insert_waiting_task(struct semaphore *sem, struct task *t)
{
	struct task *task;

	if (sem->waiting) {
		LIST_FOREACH(task, &sem->waiting_tasks, next) {
			if (t->priority > task->priority)
				LIST_INSERT_BEFORE(task, t, next);
		}

	} else {
		LIST_INSERT_HEAD(&sem->waiting_tasks, t, next);
	}


}

void init_semaphore(struct semaphore *sem, unsigned int value)
{
	sem->value = value;
	sem->count = 0;
	sem->waiting = 0;

	LIST_INIT(&sem->waiting_tasks);
}

void svc_sem_wait(struct semaphore *sem)
{
	struct task *current_task;

	if (sem->count < sem->value) {
		printf("sem (%x) got\r\n", sem);
		sem->count++;
	} else {
		printf("unable to got sem (%x)\r\n", sem);

		current_task = get_current_task();
		current_task->state = TASK_BLOCKED;

		remove_runnable_task(current_task);
		insert_waiting_task(sem, current_task);

		sem->waiting++;
		
		raise(SIGALRM);
	}
}

void sem_wait(struct semaphore *sem)
{
	svc_sem_wait(sem);
}

void svc_sem_post(struct semaphore *sem)
{
	struct task *task;

	if (sem->waiting) {
		printf("tasks are waiting for sem (%x)\r\n", sem);

		task = LIST_FIRST(&sem->waiting_tasks);
		LIST_REMOVE(task, next);
		task->state = TASK_RUNNABLE;
		sem->waiting--;
		sem->count--;

		insert_runnable_task(task);
		raise(SIGALRM);

	} else {
		if (sem->count == 0)
			printf("all sem (%x) token has been post\r\n", sem);
		else
			sem->count--;

		printf("sem (%x) post\r\n", sem);
	}
}

void sem_post(struct semaphore *sem)
{
	svc_sem_post(sem);
}
