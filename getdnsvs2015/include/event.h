/* event.h - Event management
    Copyright 2000, 2001, 2002 Free Software Foundation, Inc.
    Written by Stephane Carrez (stcarrez@worldnet.fr)

 This file is part of GEL.

 GEL is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 GEL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GEL; see the file COPYING.  If not, write to
 the Free Software Foundation, 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.  */

 #ifndef _GEL_EVENT_H
 #define _GEL_EVENT_H

#define EVLOOP_ONCE 0x01
#define EVLOOP_NONBLOCK 0x02

#define EV_TIMEOUT  0x01
#define EV_READ     0x02
#define EV_WRITE    0x04
#define EV_SIGNAL   0x08
#define EV_PERSIST  0x10
#define EV_ET       0x20

 struct event;
 struct event_callout;
 struct event_def;
 typedef struct event_def *event_type;
 typedef struct event event;
 typedef struct event_callout event_callout;

 extern unsigned long event_idle_counter;

 struct event
 {
       event_type     type;
    
           unsigned short time;
    
           unsigned short data;
     };

 typedef void(*event_handler_t) (event_callout *, event *);

 struct event_callout
 {
       struct event_callout *next;
       struct event_def     *type;
       event_handler_t       callback;
       void *data;
     };

 struct event_def
 {
       struct event_callout *callouts;
     };

 void
 event_initialize(event *queue, unsigned short size);

 void
 event_post(event_type type, unsigned short data);

#ifndef CH_WINDOWS_PORT
 void
 event_add_callout(event_type type, event_callout *callout);

 void
 event_remove_callout(event_callout *callout);

 extern void event_register(event_type type, event_callout *callout,
                                 event_handler_t handler, void *client_data);

 extern inline void
     event_register(event_type type, event_callout *callout,
         event_handler_t handler, void *client_data)
 {
     callout->callback = handler;
     callout->data = client_data;
     event_add_callout(type, callout);
 }

 extern void event_unregister(event_callout *);

 extern inline void
     event_unregister(event_callout *callout)
 {
     event_remove_callout(callout);
 }
#endif

 extern unsigned char event_false;

 extern void
 event_loop(unsigned long tick, unsigned char *bool);

 extern void
 event_wait(unsigned long nticks, event_type type);

 #endif