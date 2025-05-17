/****************************************************************************
 * apps/examples/bmi160/sixaxis_uorb_main.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <poll.h>
#include <errno.h>
#include <sensor/accel.h>
#include <sensor/gyro.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define ACC_TIMEOUT      1000

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/***************************************************************************  *
 * sixaxis_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  FAR const struct orb_metadata *accel_meta;
  struct sensor_accel accel_data;
  struct pollfd fds;
  int ret = OK;
  int fd;

  accel_meta = ORB_ID(sensor_accel);
  fd = orb_subscribe_multi(accel_meta, 0);
  if (fd < 0)
    {
      printf("sensor accel subscribe error! return:%d\n", fd);
      return fd;
    }

  orb_set_frequency(fd, 100);

  fds.fd     = fd;
  fds.events = POLLIN;

  while(1)
    {
      if (poll(&fds, 1, ACC_TIMEOUT) > 0)
        {
          if (fds.revents & POLLIN)
            {
              ret = orb_copy(accel_meta, fd, &accel_data);
#ifdef CONFIG_DEBUG_UORB
              if (ret == OK && accel_meta->o_format != NULL)
                {
                  orb_info(accel_meta->o_format, accel_meta->o_name, &accel_data);
                }
#endif
            }
        }
      else if (errno != EINTR)
        {
          printf("Waited for %d milliseconds without a message. "
                 "Giving up. err: %d\n", ACC_TIMEOUT, errno);
          break;
        }
    }

  orb_unsubscribe(fd);
  printf("sensor accel read examples exit.\n");

  return ret;
}
