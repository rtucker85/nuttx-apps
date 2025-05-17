/****************************************************************************
 * apps/examples/bmp280/bmp280_main.c
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

#if 0

#include <nuttx/config.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <nuttx/sensors/sensor.h>

#else

#include <poll.h>
#include <errno.h>
#include <sensor/baro.h>

#define BARO_TIMEOUT 1000

#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * bmp280_main
 ****************************************************************************/

 #if 0

int main(int argc, FAR char *argv[])
{
  int fd;
  int ret;
  struct sensor_baro sensor_data;

  fd = open("/dev/uorb/sensor_baro0", O_RDONLY | O_NONBLOCK);
  if (fd < 0)
    {
      printf("Failed to open BMP280 sensor\n");
      return EXIT_FAILURE;
    }

  while(1)
  {
    ret = read(fd, &sensor_data, sizeof(sensor_data));
    if (ret != sizeof(sensor_data))
      {
        perror("Could not read");
        return EXIT_FAILURE;
      }

  #if 0
    printf("Absolute pressure [hPa] = %f\n", sensor_data.pressure);
    printf("Temperature [C] = %f\n", sensor_data.temperature);
  #endif

  #if 0
    printf("timestamp:%" PRIu64 ",pressure:%h,temperature:%h\n",
      sensor_data.timestamp,
      sensor_data.pressure,
      sensor_data.temperature
    );
  #endif

  #if 1
    char name[] = "baro0";
    printf("%s: timestamp:%" PRIu64 " pressure:%.2f temperature:%.2f\n",
      name, sensor_data.timestamp, sensor_data.pressure, sensor_data.temperature);
  #endif

    usleep(1000 * 1000);
  }

  return EXIT_SUCCESS;
}

#else

int main(int argc, FAR char *argv[])
{
  FAR const struct orb_metadata *baro_meta;
  struct sensor_baro baro_data;
  struct pollfd fds;
  int ret = OK;
  int fd;

  baro_meta = ORB_ID(sensor_baro);
  fd = orb_subscribe_multi(baro_meta, 0);
  if (fd < 0)
    {
      printf("sensor barometer subscribe error! return:%d\n", fd);
      return fd;
    }

  orb_set_frequency(fd, 100);

  fds.fd     = fd;
  fds.events = POLLIN;

  while(1)
    {
      if (poll(&fds, 1, BARO_TIMEOUT) > 0)
        {
          if (fds.revents & POLLIN)
            {
              ret = orb_copy(baro_meta, fd, &baro_data);
#ifdef CONFIG_DEBUG_UORB
              if (ret == OK && baro_meta->o_format != NULL)
                {
                  orb_info(baro_meta->o_format, baro_meta->o_name, &baro_data);
                }
#endif
            }
        }
      else if (errno != EINTR)
        {
          printf("Waited for %d milliseconds without a message. "
                 "Giving up. err: %d\n", BARO_TIMEOUT, errno);
          break;
        }
    }

  orb_unsubscribe(fd);
  printf("sensor barometers read examples exit.\n");

  return ret;
}

#endif
