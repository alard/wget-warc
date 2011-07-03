
/* ------------------------------------------------------------------- */
/* Copyright (c) 2007-2008 Hanzo Archives Limited.                     */
/*                                                                     */
/* Licensed under the Apache License, Version 2.0 (the "License");     */
/* you may not use this file except in compliance with the License.    */
/* You may obtain a copy of the License at                             */
/*                                                                     */
/*     http://www.apache.org/licenses/LICENSE-2.0                      */
/*                                                                     */
/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS,   */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or     */
/* implied.                                                            */
/* See the License for the specific language governing permissions and */
/* limitations under the License.                                      */
/*                                                                     */
/* You may find more information about Hanzo Archives at               */
/*                                                                     */
/*     http://www.hanzoarchives.com/                                   */
/*                                                                     */
/* You may find more information about the WARC Tools project at       */
/*                                                                     */
/*     http://code.google.com/p/warc-tools/                            */
/* ------------------------------------------------------------------- */

#ifndef	__FSM_X__
#define	__FSM_X__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif

    /**
     * Portability header file
     */

#include <wport.h>
#include <wmisc.h>

    typedef const struct eventAct
      {
        warc_bool_t (* thisEvent) (void *);
        void    (* action)    (void *);

        const struct eventAct * newState;
      } Transition;

    typedef Transition State[];


#ifdef __cplusplus
  }

#endif

#endif /* __FSM_X__ */
