/*
 * Copyright (C) 2003-2010 The Paparazzi Team
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

/** \file hitl_transport.h
 *  \brief Interface for downlink transport implementation
 *
 */

#ifndef  HITL_TRANSPORT_H
#define  HITL_TRANSPORT_H
#include <QtCore/QtGlobal>
#include "inttypes.h"
#include "downlink_transport.h"
#include "downlink.h"
#include "common.h"


template<class T>class QSynchQueue;


class HitlTransport : public DownlinkTransport
{

 public:
  HitlTransport(QSynchQueue<ethernetMessage> *pqueue){
    /*Though these must be done in initiliser list.*/
    InitData();
    ptransmitqueue = pqueue;

  }
 public:
  uint8_t SizeOf (void *impl, uint8_t size);
  int CheckFreeSpace(void *impl, uint8_t size);
  void PutBytes(void *impl, enum DownlinkDataType data_type, uint8_t len, const void *bytes);
  void StartMessage(void *impl,const char *name, uint8_t msg_id, uint8_t payload_len);
  void EndMessage(void *impl);
  void Overrun(void *impl);
  void CountBytes(void *impl, uint8_t len);
  void Periodic(void *impl);

 private:
  void Fn_Transmit(uint8_t _x);
  bool_t Fn_CheckFreeSpace(uint8_t _x);
  void Fn_SendMessage();
  void InitData();
 private:
  char buffer[MAX_CHARS];
  quint16 cur_index;
  QSynchQueue<ethernetMessage>*ptransmitqueue;
  quint8 ck_a,ck_b;

  uint8_t downlink_nb_ovrn;
  uint8_t downlink_nb_bytes;
  uint8_t downlink_nb_msgs;
};

#endif /* DOWNLINK_TRANSPORT_H */
