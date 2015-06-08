
// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "rgw_goog_auth.h"
#include "rgw_rest.h"

#include "common/ceph_crypto.h"
#include "common/Clock.h"

#include "auth/Crypto.h"

#include "rgw_client_io.h"
#include <uuid/uuid.h>

#define dout_subsys ceph_subsys_rgw

#define DEFAULT_GOOG_PREFIX "goog"

using namespace ceph::crypto;


void RGW_GOOG_Auth_Get::execute()
{
  int ret = -EPERM;

  bufferlist bl;
  uuid_t state_id;
  uuid_generate(state_id);
  //s->goog_oauth_state_id = state_id;
  memcpy(s->goog_oauth_state_id,state_id,sizeof(state_id));
  ret = ERR_PERMANENT_REDIRECT;   
  set_req_state_err(s, ret);

}

void RGW_GOOG_Auth_Get::send_response()
{
  dump_errno(s);
  end_header(s,this,"application/xml");
  dump_start(s);
  //std::string redirect = "https://oauth.io/auth/google?k=lQ8NFm40FMCSc1FOK0leMixH0Jk&opts="+"{\"stat\":\""+s->goog_oauth_state_id+ "\"}&redirect_type=server&redirect_uri=http://localhost:3000/oauth/redirect";
  std::string redirect = "https://oauth.io/auth/google?k=lQ8NFm40FMCSc1FOK0leMixH0Jk&opts={\"stat\":\"";
  std::string state = std::string(reinterpret_cast<const char*>(s->goog_oauth_state_id), sizeof(s->goog_oauth_state_id)/sizeof(s->goog_oauth_state_id[0]));
  redirect += state;
  redirect += "\"}&redirect_type=server&redirect_uri=http://localhost:3000/oauth/redirect";
  cout<< redirect;
  s->formatter->open_object_section_in_ns("a",redirect.c_str());
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s,s->formatter);
}
int RGWHandler_GOOG_Auth::init(RGWRados *store, struct req_state *state, RGWClientIO *cio)
{
  state->dialect = "goog-auth";
  state->formatter = new XMLFormatter(false);
  state->format = RGW_FORMAT_XML;

  return RGWHandler::init(store, state, cio);
}

int RGWHandler_GOOG_Auth::authorize()
{
  return 0;
}

RGWOp *RGWHandler_GOOG_Auth::op_get()
{
  return new RGW_GOOG_Auth_Get;
}

