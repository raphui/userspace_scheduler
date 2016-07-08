/*
 * Copyright (C) 2016  Raphaël Poggi <poggi.raph@gmail.com>
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
#include <stdlib.h>
#include <unistd.h>

#include <semaphore.h>
#include <scheduler.h>

static struct semaphore sem;

void first_task(void)
{
	int i = 0;

	sem_wait(&sem);
	while (1) {
//		printf("A");
#if defined(SCHEDULE_PRIORITY) || defined(SCHEDULE_ROUND_ROBIN)
		if (i++ == 10) {
			sem_wait(&sem);
			i = 0;
		}
#endif
	}
}

void second_task(void)
{
	int i = 0;

	while (1) {
//		printf("B");
		if (i++ == 20) {
#if defined(SCHEDULE_PRIORITY) || defined(SCHEDULE_ROUND_ROBIN)
			sem_post(&sem);
//#elif defined(SCHEDULE_ROUND_ROBIN)
//			sem_wait(&sem);
#endif
			i = 0;
		}
	}
}

int main(int argc, char **argv)
{
	printf("init sem\n");
	init_semaphore(&sem, 1);

	printf("init scheduling\n");
	schedule_init();

	printf("adding task\n");
	add_task(&first_task, 10);
	add_task(&second_task, 3);

	printf("start scheduling\n");
	start_schedule();

	while (1)
		;

	return 0;
}
