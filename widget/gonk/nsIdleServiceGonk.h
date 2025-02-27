/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim:expandtab:shiftwidth=4:tabstop=4: */
/* Copyright 2012 Mozilla Foundation and Mozilla contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef nsIdleServiceGonk_h__
#define nsIdleServiceGonk_h__

#include "nsUserIdleService.h"

class nsIdleServiceGonk : public nsUserIdleService {
 public:
  NS_DECL_ISUPPORTS_INHERITED

  bool PollIdleTime(uint32_t* aIdleTime) override;

  static already_AddRefed<nsIdleServiceGonk> GetInstance() {
    RefPtr<nsIdleServiceGonk> idleService =
        nsUserIdleService::GetInstance().downcast<nsIdleServiceGonk>();
    if (!idleService) {
      idleService = new nsIdleServiceGonk();
    }

    return idleService.forget();
  }

 protected:
  nsIdleServiceGonk() {}
  virtual ~nsIdleServiceGonk() {}
};

#endif  // nsIdleServiceGonk_h__
