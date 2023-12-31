/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file m2ap_handler.h
 * \brief m2ap handler procedures for eNB
 * \author Javier Morgade <javier.morgade@ieee.org>
 * \date 2019
 * \version 0.1
 */

#ifndef M2AP_ENB_OLD_HANDLERS_H_
#define M2AP_ENB_OLD_HANDLERS_H_

#include "m2ap_eNB_defs.h"

void m2ap_handle_m2_setup_message(m2ap_eNB_instance_t *instance_p, m2ap_eNB_data_t *eNB_desc_p, int sctp_shutdown);

int m2ap_eNB_handle_message(instance_t instance,
                            sctp_assoc_t assoc_id,
                            int32_t stream,
                            const uint8_t *const data,
                            const uint32_t data_length);

int m2ap_handle_message(instance_t instance,
                        sctp_assoc_t assoc_id,
                        int32_t stream,
                        const uint8_t *const data,
                        const uint32_t data_length);

#endif /* M2AP_ENB_OLD_HANDLERS_H_ */
