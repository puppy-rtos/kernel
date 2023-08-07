/****************************************************************************
 * mm/kasan/kasan.c
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

#include <puppy.h>

#define KLOG_TAG  "kasan"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "kasan.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define KASAN_BYTES_PER_WORD (sizeof(uintptr_t))
#define KASAN_BITS_PER_WORD  (KASAN_BYTES_PER_WORD * 8)

#define KASAN_FIRST_WORD_MASK(start) \
  (UINTPTR_MAX << ((start) & (KASAN_BITS_PER_WORD - 1)))
#define KASAN_LAST_WORD_MASK(end) \
  (UINTPTR_MAX >> (-(end) & (KASAN_BITS_PER_WORD - 1)))

#define KASAN_SHADOW_SCALE (sizeof(uintptr_t))

#define KASAN_SHADOW_SIZE(size) \
  (KASAN_BYTES_PER_WORD * ((size) / KASAN_SHADOW_SCALE / KASAN_BITS_PER_WORD))
#define KASAN_REGION_SIZE(size) \
  (sizeof(struct kasan_region_s) + KASAN_SHADOW_SIZE(size))

#define KASAN_INIT_VALUE            0xDEADCAFE

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct kasan_region_s
{
  struct kasan_region_s *next;
  uintptr_t                  begin;
  uintptr_t                  end;
  uintptr_t                  shadow[1];
};

/****************************************************************************
 * Private Data
 ****************************************************************************/


static struct kasan_region_s *g_region;
static uint32_t g_region_init;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static uintptr_t *kasan_mem_to_shadow(const void *ptr, size_t size,
                                          unsigned int *bit)
{
  struct kasan_region_s *region;
  uintptr_t addr = (uintptr_t)ptr;

  if (g_region_init != KASAN_INIT_VALUE)
    {
      return NULL;
    }

  for (region = g_region; region != NULL; region = region->next)
    {
      if (addr >= region->begin && addr < region->end)
        {
          KLOG_ASSERT(addr + size <= region->end);
          addr -= region->begin;
          addr /= KASAN_SHADOW_SCALE;
          *bit  = addr % KASAN_BITS_PER_WORD;
          return &region->shadow[addr / KASAN_BITS_PER_WORD];
        }
    }

  return NULL;
}

static void kasan_report(const void *addr, size_t size, bool is_write)
{
  static int recursion;

  if (++recursion == 1)
    {
      KLOG_E("kasan detected a %s access error, address at %0x, size is %d\n", is_write ? "write" : "read", (uintptr_t)addr, size);
      KLOG_ASSERT(0);
    }

  --recursion;
}

static bool kasan_is_poisoned(const void *addr, size_t size)
{
  uintptr_t *p;
  unsigned int bit;

  p = kasan_mem_to_shadow(addr + size - 1, 1, &bit);
  return p && ((*p >> bit) & 1);
}

static void kasan_set_poison(const void *addr, size_t size,
                             bool poisoned)
{
  uintptr_t *p;
  unsigned int bit;
  unsigned int nbit;
  uintptr_t mask;
  
  p_base_t key = arch_irq_lock();
  arch_spin_lock(&cpu);

  p = kasan_mem_to_shadow(addr, size, &bit);
  KLOG_ASSERT(p != NULL);

  nbit = KASAN_BITS_PER_WORD - bit % KASAN_BITS_PER_WORD;
  mask = KASAN_FIRST_WORD_MASK(bit);

  size /= KASAN_SHADOW_SCALE;
  while (size >= nbit)
    {
      if (poisoned)
        {
          *p++ |= mask;
        }
      else
        {
          *p++ &= ~mask;
        }

      bit  += nbit;
      size -= nbit;

      nbit = KASAN_BITS_PER_WORD;
      mask = UINTPTR_MAX;
    }

  if (size)
    {
      mask &= KASAN_LAST_WORD_MASK(bit + size);
      if (poisoned)
        {
          *p |= mask;
        }
      else
        {
          *p &= ~mask;
        }
    }

  arch_spin_unlock(&cpu);
  arch_irq_unlock(key);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* Exported functions called from other mm module */

void kasan_poison(const void *addr, size_t size)
{
  kasan_set_poison(addr, size, true);
}

void kasan_unpoison(const void *addr, size_t size)
{
  kasan_set_poison(addr, size, false);
}

void kasan_register(void *addr, size_t *size)
{
  struct kasan_region_s *region;
  p_base_t key;

  region = (struct kasan_region_s *)
    ((char *)addr + *size - KASAN_REGION_SIZE(*size));

  region->begin = (uintptr_t)addr;
  region->end   = region->begin + *size;

  key = arch_irq_lock();
  arch_spin_lock(&cpu);
  region->next  = g_region;
  g_region      = region;
  g_region_init = KASAN_INIT_VALUE;

  arch_spin_unlock(&cpu);
  arch_irq_unlock(key);

  kasan_poison(addr, *size);
  *size -= KASAN_REGION_SIZE(*size);
}

/* Exported functions called from the compiler generated code */

void __sanitizer_annotate_contiguous_container(const void *beg,
                                               const void *end,
                                               const void *old_mid,
                                               const void *new_mid)
{
  /* Shut up compiler complaints */
}

void __asan_before_dynamic_init(const void *module_name)
{
  /* Shut up compiler complaints */
}

void __asan_after_dynamic_init(void)
{
  /* Shut up compiler complaints */
}

void __asan_handle_no_return(void)
{
  /* Shut up compiler complaints */
}

void __asan_report_load_n_noabort(void *addr, size_t size)
{
  kasan_report(addr, size, false);
}

void __asan_report_store_n_noabort(void *addr, size_t size)
{
  kasan_report(addr, size, true);
}

void __asan_report_load16_noabort(void *addr)
{
  __asan_report_load_n_noabort(addr, 16);
}

void __asan_report_store16_noabort(void *addr)
{
  __asan_report_store_n_noabort(addr, 16);
}

void __asan_report_load8_noabort(void *addr)
{
  __asan_report_load_n_noabort(addr, 8);
}

void __asan_report_store8_noabort(void *addr)
{
  __asan_report_store_n_noabort(addr, 8);
}

void __asan_report_load4_noabort(void *addr)
{
  __asan_report_load_n_noabort(addr, 4);
}

void __asan_report_store4_noabort(void *addr)
{
  __asan_report_store_n_noabort(addr, 4);
}

void __asan_report_load2_noabort(void *addr)
{
  __asan_report_load_n_noabort(addr, 2);
}

void __asan_report_store2_noabort(void *addr)
{
  __asan_report_store_n_noabort(addr, 2);
}

void __asan_report_load1_noabort(void *addr)
{
  __asan_report_load_n_noabort(addr, 1);
}

void __asan_report_store1_noabort(void *addr)
{
  __asan_report_store_n_noabort(addr, 1);
}

void __asan_loadN_noabort(void *addr, size_t size)
{
  if (kasan_is_poisoned(addr, size))
    {
      kasan_report(addr, size, false);
    }
}

void __asan_storeN_noabort(void * addr, size_t size)
{
  if (kasan_is_poisoned(addr, size))
    {
      kasan_report(addr, size, true);
    }
}

void __asan_load16_noabort(void *addr)
{
  __asan_loadN_noabort(addr, 16);
}

void __asan_store16_noabort(void *addr)
{
  __asan_storeN_noabort(addr, 16);
}

void __asan_load8_noabort(void *addr)
{
  __asan_loadN_noabort(addr, 8);
}

void __asan_store8_noabort(void *addr)
{
  __asan_storeN_noabort(addr, 8);
}

void __asan_load4_noabort(void *addr)
{
  __asan_loadN_noabort(addr, 4);
}

void __asan_store4_noabort(void *addr)
{
  __asan_storeN_noabort(addr, 4);
}

void __asan_load2_noabort(void *addr)
{
  __asan_loadN_noabort(addr, 2);
}

void __asan_store2_noabort(void *addr)
{
  __asan_storeN_noabort(addr, 2);
}

void __asan_load1_noabort(void *addr)
{
  __asan_loadN_noabort(addr, 1);
}

void __asan_store1_noabort(void *addr)
{
  __asan_storeN_noabort(addr, 1);
}

void __asan_loadN(void *addr, size_t size)
{
  __asan_loadN_noabort(addr, size);
}

void __asan_storeN(void *addr, size_t size)
{
  __asan_storeN_noabort(addr, size);
}

void __asan_load16(void *addr)
{
  __asan_load16_noabort(addr);
}

void __asan_store16(void *addr)
{
  __asan_store16_noabort(addr);
}

void __asan_load8(void *addr)
{
  __asan_load8_noabort(addr);
}

void __asan_store8(void *addr)
{
  __asan_store8_noabort(addr);
}

void __asan_load4(void *addr)
{
  __asan_load4_noabort(addr);
}

void __asan_store4(void *addr)
{
  __asan_store4_noabort(addr);
}

void __asan_load2(void *addr)
{
  __asan_load2_noabort(addr);
}

void __asan_store2(void *addr)
{
  __asan_store2_noabort(addr);
}

void __asan_load1(void *addr)
{
  __asan_load1_noabort(addr);
}

void __asan_store1(void *addr)
{
  __asan_store1_noabort(addr);
}
