/*
 * Copyright (c) 2017, 2020, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#include <string.h>
#include <math.h>
#include <errno.h>
#include "runtime/globals.hpp"
#include "runtime/os.hpp"
#include "runtime/globals_extension.hpp"
#include "osContainer_linux.hpp"
#include "cgroupSubsystem_linux.hpp"


bool  OSContainer::_is_initialized   = false;
bool  OSContainer::_is_containerized = false;

CgroupSubsystem* cgroup_subsystem;

/* init
 *
 * Initialize the container support and determine if
 * we are running under cgroup control.
 */
void OSContainer::init() {
  assert(!_is_initialized, "Initializing OSContainer more than once");

  _is_initialized = true;
  _is_containerized = false;

  if (PrintContainerInfo) {
    tty->print_cr("OSContainer::init: Initializing Container Support");
  }
  if (CPUShareScaleFactor > 1 && !UseContainerSupport) {
    FLAG_SET_DEFAULT(UseContainerSupport, true);
  }
  if (!UseContainerSupport) {
    if (PrintContainerInfo) {
      tty->print_cr("Container Support not enabled");
    }
    return;
  }
  uintptr_t core_limit = (uintptr_t)os::Linux::active_processor_count();
  if (CPUShareScaleLimit > core_limit || CPUShareScaleLimit == 0) {
    FLAG_SET_DEFAULT(CPUShareScaleLimit, core_limit);
  }
  if (CPUShareScaleFactor == 0) {
    FLAG_SET_DEFAULT(CPUShareScaleFactor, 1);
  }
  if (PrintContainerInfo) {
    tty->print_cr("CPUShareScaleLimit is %d", (int)CPUShareScaleLimit);
    tty->print_cr("CPUShareScaleFactor is %d", (int)CPUShareScaleFactor);
  }

  cgroup_subsystem = CgroupSubsystemFactory::create();
  if (cgroup_subsystem == NULL) {
    return; // Required subsystem files not found or other error
  }

  _is_containerized = true;
}

const char * OSContainer::container_type() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->container_type();
}

jlong OSContainer::memory_limit_in_bytes() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->memory_limit_in_bytes();
}

jlong OSContainer::memory_and_swap_limit_in_bytes() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->memory_and_swap_limit_in_bytes();
}

jlong OSContainer::memory_soft_limit_in_bytes() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->memory_soft_limit_in_bytes();
}

/* memory_usage_in_bytes
 *
 * Return the amount of used memory for this process.
 *
 * return:
 *    memory usage in bytes or
 *    -1 for unlimited
 *    OSCONTAINER_ERROR for not supported
 */
jlong OSContainer::memory_usage_in_bytes() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->memory_usage_in_bytes();
}

jlong OSContainer::memory_max_usage_in_bytes() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->memory_max_usage_in_bytes();
}

char * OSContainer::cpu_cpuset_cpus() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->cpu_cpuset_cpus();
}

char * OSContainer::cpu_cpuset_memory_nodes() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->cpu_cpuset_memory_nodes();
}

int OSContainer::active_processor_count() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->active_processor_count();
}

int OSContainer::cpu_quota() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->cpu_quota();
}

int OSContainer::cpu_period() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->cpu_period();
}

int OSContainer::cpu_shares() {
  assert(cgroup_subsystem != NULL, "cgroup subsystem not available");
  return cgroup_subsystem->cpu_shares();
}
