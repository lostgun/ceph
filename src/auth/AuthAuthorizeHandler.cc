// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2009-2011 New Dream Network
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include "AuthAuthorizeHandler.h"
#include "cephx/CephxAuthorizeHandler.h"
#include "krb/KrbAuthorizeHandler.hpp"
#include "none/AuthNoneAuthorizeHandler.h"

AuthAuthorizeHandler *AuthAuthorizeHandlerRegistry::get_handler(int protocol)
{
  if (!supported.is_supported_auth(protocol)) {
    return NULL;
  }
  
  std::scoped_lock l{m_lock};
  map<int,AuthAuthorizeHandler*>::iterator iter = m_authorizers.find(protocol);
  if (iter != m_authorizers.end())
    return iter->second;

  switch (protocol) {
  case CEPH_AUTH_NONE:
    m_authorizers[protocol] = new AuthNoneAuthorizeHandler();
    return m_authorizers[protocol];
    
  case CEPH_AUTH_CEPHX:
    m_authorizers[protocol] = new CephxAuthorizeHandler();
    return m_authorizers[protocol];

  case CEPH_AUTH_GSS:
    m_authorizers[protocol] = new KrbAuthorizeHandler();
    return m_authorizers[protocol];
  }
  return NULL;
}

AuthAuthorizeHandlerRegistry::~AuthAuthorizeHandlerRegistry()
{
  for (map<int,AuthAuthorizeHandler*>::iterator iter = m_authorizers.begin();
       iter != m_authorizers.end();
       ++iter)
    delete iter->second;
}
