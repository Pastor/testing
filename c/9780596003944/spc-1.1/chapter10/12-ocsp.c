spc_ocspresult_t spc_verify_via_ocsp(spc_ocsprequest_t *data) {
  BIO                   *bio = 0;
  int                   rc, reason, ssl, status;
  char                  *host = 0, *path = 0, *port = 0;
  SSL_CTX               *ctx = 0;
  X509_STORE            *store = 0;
  OCSP_CERTID           *id;
  OCSP_REQUEST          *req = 0;
  OCSP_RESPONSE         *resp = 0;
  OCSP_BASICRESP        *basic = 0;
  spc_ocspresult_t      result;
  ASN1_GENERALIZEDTIME  *producedAt, *thisUpdate, *nextUpdate;

  result = SPC_OCSPRESULT_ERROR_UNKNOWN;
  if (!OCSP_parse_url(data->url, &host, &port, &path, &ssl)) {
    result = SPC_OCSPRESULT_ERROR_BADOCSPADDRESS;
    goto end;
  }
  if (!(req = OCSP_REQUEST_new())) {
    result = SPC_OCSPRESULT_ERROR_OUTOFMEMORY;
    goto end;
  }

  id = OCSP_cert_to_id(0, data->cert, data->issuer);
  if (!id || !OCSP_request_add0_id(req, id)) goto end;
  OCSP_request_add1_nonce(req, 0, -1);

  /* sign the request */
  if (data->sign_cert && data->sign_key &&
      !OCSP_request_sign(req, data->sign_cert, data->sign_key, EVP_sha1(), 0, 0)) {
    result = SPC_OCSPRESULT_ERROR_SIGNFAILURE;
    goto end;
  }

  /* establish a connection to the OCSP responder */
  if (!(bio = spc_connect(host, atoi(port), ssl, data->store, &ctx))) {
    result = SPC_OCSPRESULT_ERROR_CONNECTFAILURE;
    goto end;
  }

  /* send the request and get a response */
  resp = OCSP_sendreq_bio(bio, path, req);
  if ((rc = OCSP_response_status(resp)) != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    switch (rc) {
      case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST:
        result = SPC_OCSPRESULT_ERROR_MALFORMEDREQUEST; break;
      case OCSP_RESPONSE_STATUS_INTERNALERROR:
        result = SPC_OCSPRESULT_ERROR_INTERNALERROR;    break;
      case OCSP_RESPONSE_STATUS_TRYLATER:
        result = SPC_OCSPRESULT_ERROR_TRYLATER;         break;
      case OCSP_RESPONSE_STATUS_SIGREQUIRED:
        result = SPC_OCSPRESULT_ERROR_SIGREQUIRED;      break;
      case OCSP_RESPONSE_STATUS_UNAUTHORIZED:
        result = SPC_OCSPRESULT_ERROR_UNAUTHORIZED;     break;
    }
    goto end;
  }

  /* verify the response */
  result = SPC_OCSPRESULT_ERROR_INVALIDRESPONSE;
  if (!(basic = OCSP_response_get1_basic(resp))) goto end;
  if (OCSP_check_nonce(req, basic) <= 0) goto end;
  if (data->store && !(store = spc_create_x509store(data->store))) goto end;
  if ((rc = OCSP_basic_verify(basic, 0, store, 0)) <= 0) goto end;

  if (!OCSP_resp_find_status(basic, id, &status, &reason, &producedAt,
                             &thisUpdate, &nextUpdate))
    goto end;
  if (!OCSP_check_validity(thisUpdate, nextUpdate, data->skew, data->maxage))
    goto end;

  /* All done.  Set the return code based on the status from the response. */
  if (status == V_OCSP_CERTSTATUS_REVOKED)
    result = SPC_OCSPRESULT_CERTIFICATE_REVOKED;
  else
    result = SPC_OCSPRESULT_CERTIFICATE_VALID;

end:
  if (bio) BIO_free_all(bio);
  if (host) OPENSSL_free(host);
  if (port) OPENSSL_free(port);
  if (path) OPENSSL_free(path);
  if (req) OCSP_REQUEST_free(req);
  if (resp) OCSP_RESPONSE_free(resp);
  if (basic) OCSP_BASICRESP_free(basic);
  if (ctx) SSL_CTX_free(ctx);
  if (store) X509_STORE_free(store);
  return result;
}
