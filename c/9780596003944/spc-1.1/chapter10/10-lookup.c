static spc_cacert_t lookup_table[] = {
  { "Equifax Secure Certificate Authority",
    "\x67\xcb\x9d\xc0\x13\x24\x8a\x82\x9b\xb2\x17\x1e\xd1\x1b\xec\xd4", 16,
    "http://crl.geotrust.com/crls/secureca.crl",
  },
  { "Equifax Secure Global eBusiness CA-1",
    "\x8f\x5d\x77\x06\x27\xc4\x98\x3c\x5b\x93\x78\xe7\xd7\x7d\x9b\xcc", 16,
    "http://crl.geotrust.com/crls/globalca1.crl",
  },
  { "Equifax Secure eBusiness CA-1",
    "\x64\x9c\xef\x2e\x44\xfc\xc6\x8f\x52\x07\xd0\x51\x73\x8f\xcb\x3d", 16,
    "http://crl.geotrust.com/crls/ebizca1.crl",
  },
  { "Equifax Secure eBusiness CA-2",
    "\xaa\xbf\xbf\x64\x97\xda\x98\x1d\x6f\xc6\x08\x3a\x95\x70\x33\xca", 16,
    "http://crl.geotrust.com/crls/ebiz.crl",
  },
  { "RSA Data Security Secure Server CA (VeriSign)",
    "\x74\x7b\x82\x03\x43\xf0\x00\x9e\x6b\xb3\xec\x47\xbf\x85\xa5\x93", 16,
    "http://crl.verisign.com/RSASecureServer.crl", "http://ocsp.verisign.com/",
  },
  { "Thawte Server CA",
    "\xc5\x70\xc4\xa2\xed\x53\x78\x0c\xc8\x10\x53\x81\x64\xcb\xd0\x1d", 16,
    "https://www.thawte.com/cgi/lifecycle/ getcrl.crl?skeyid=%07%15%28mps%AA"
    "%B2%8A%7C%0F%86%CE8%93%008%05%8A%B1",
  },
  { "TrustCenter Class 1 CA",
    "\x8d\x26\xff\x2f\x31\x6d\x59x\29\xdd\xe6\x36\xa7\xe2\xce\x64\x25", 16,
    "https://www.trustcenter.de:443/cgi-bin/CRL.cgi/ TC_Class1.crl?Page=GetCrl"
    "&crl=2",
  },
  { "TrustCenter Class 2 CA",
    "\xb8\x16\x33\x4c\x4c\x4c\xf2\xd8\xd3\x4d\x06\xb4\xa6\x58\x40\x03", 16,
    "https://www.trustcenter.de:443/cgi-bin/CRL.cgi/ TC_Class2.crl?Page=GetCrl"
    "&crl=3",
  },
  { "TrustCenter Class 3 CA",
    "\x5f\x94\x4a\x73\x22\xb8\xf7\xd1\x31\xec\x59\x39\xf7\x8e\xfe\x6e", 16,
    "https://www.trustcenter.de:443/cgi-bin/CRL.cgi/ TC_Class3.crl?Page=GetCrl"
    "&crl=4",
  },
  { "TrustCenter Class 4 CA",
    "\x0e\xfa\x4b\xf7\xd7\x60\xcd\x65\xf7\xa7\x06\x88\x57\x98\x62\x39", 16,
    "https://www.trustcenter.de:443/cgi-bin/CRL.cgi/ TC_Class4.crl?Page=GetCrl"
    "&crl=5",
  },
  { "The USERTRUST Network - UTN-UserFirst-Object",
    "\xa7\xf2\xe4\x16\x06\x41\x11\x60\x30\x6b\x9c\xe3\xb4\x9c\xb0\xc9", 16,
    "http://crl.usertrust.com/UTN-UserFirst-Object.crl",
  },
  { "The USERTRUST Network - UTN-UserFirst-Network Applications",
    "\xbf\x60\x59\xa3\x5b\xba\xf6\xa7\x76\x42\xda\x6f\x1a\x7b\x50\xcf", 16,
    "http://crl.usertrust.com/UTN-UserFirst-NetworkApplications.crl",
  },
  { "The USERTRUST Network - UTN-UserFirst-Hardware",
    "\x4c\x56\x41\xe5\x0d\xbb\x2b\xe8\xca\xa3\xed\x18\x08\xad\x43\x39", 16,
    "http://crl.usertrust.com/UTN-UserFirst-Hardware.crl",
  },
  { "The USERTRUST Network - UTN-UserFirst-Client Authentication and Email",
    "\xd7\x34\x3d\xef\x1d\x27\x09\x28\xe1\x31\x02\x5b\x13\x2b\xdd\xf7", 16,
    "http://crl.usertrust.com/UTN-UserFirst- ClientAuthenticationandEmail.crl",
  },
  { "The USERTRUST Network - UTN - DataCorp SGC",
    "\xb3\xa5\x3e\x77\x21\x6d\xac\x4a\xc0\xc9\xfb\xd5\x41\x3d\xca\x06", 16,
    "http://crl.usertrust.com/UTN-DataCorpSGC.crl",
  },
  { "ValiCert Class 1 Policy Validation Authority",
    "\x65\x58\xab\x15\xad\x57\x6c\x1e\xa8\xa7\xb5\x69\xac\xbf\xff\xeb", 16,
    "http://www.valicert.com/repository/ ValiCert%20Calss%201%20Policy%20Val"
    "idation%20Authority.crl",
  },
  { "VeriSign Class 1 Public PCA (2020-01-07)",
    "\x51\x86\xe8\x1f\xbc\xb1\xc3\x71\xb5\x18\x10\xdb\x5f\xdc\xf6\x20", 16,
    "http://crl.verisign.com/pca1.1.1.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 1 Public PCA (2028-08-01)",
    "\x97\x60\xe8\x57\x5f\xd3\x50\x47\xe5\x43\x0c\x94\x36\x8a\xb0\x62", 16,
    "http://crl.verisign.com/pca1.1.1.crl",
    "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 1 Public PCA G2 (2018-05-18)",
    "\xf2\x7d\xe9\x54\xe4\xa3\x22\x0d\x76\x9f\xe7\x0b\xbb\xb3\x24\x2b", 16,
    "http://crl.verisign.com/pca1-g2.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 1 Public PCA G2 (2028-08-01)",
    "\xdb\x23\x3d\xf9\x69\xfa\x4b\xb9\x95\x80\x44\x73\x5e\x7d\x41\x83", 16,
    "http://crl.verisign.com/pca1-g2.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 2 Public PCA (2004-01-07)",
    "\xec\x40\x7d\x2b\x76\x52\x67\x05\x2c\xea\xf2\x3a\x4f\x65\xf0\xd8", 16,
    "http://crl.verisign.com/pca2.1.1.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 2 Public PCA (2028-08-01)",
    "\xb3\x9c\x25\xb1\xc3\x2e\x32\x53\x80\x15\x30\x9d\x4d\x02\x77\x3e", 16,
    "http://crl.verisign.com/pca2.1.1.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 2 Public PCA G2 (2018-05-18)",
    "\x74\xa8\x2c\x81\x43\x2b\x35\x60\x9b\x78\x05\x6b\x58\xf3\x65\x82", 16,
    "http://crl.verisign.com/pca2-g2.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 2 Public PCA G2 (2028-08-01)",
    "\x2d\xbb\xe5\x25\xd3\xd1\x65\x82\x3a\xb7\x0e\xfa\xe6\xeb\xe2\xe1", 16,
    "http://crl.verisign.com/pca2-g2.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 3 Public PCA (2004-01-07)",
    "\x78\x2a\x02\xdf\xdb\x2e\x14\xd5\xa7\x5f\x0a\xdf\xb6\x8e\x9c\x5d", 16,
    "http://crl.verisign.com/pca3.1.1.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 3 Public PCA (2028-08-01)",
    "\x10\xfc\x63\x5d\xf6\x26\x3e\x0d\xf3\x25\xbe\x5f\x79\xcd\x67\x67", 16,
    "http://crl.verisign.com/pca3.1.1.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 3 Public PCA G2 (2018-05-18)",
    "\xc4\x63\xab\x44\x20\x1c\x36\xe4\x37\xc0\x5f\x27\x9d\x0f\x6f\x6e", 16,
    "http://crl.verisign.com/pca3-g2.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Class 3 Public PCA G2 (2028-08-01)",
    "\xa2\x33\x9b\x4c\x74\x78\x73\xd4\x6c\xe7\xc1\xf3\x8d\xcb\x5c\xe9", 16,
    "http://crl.verisign.com/pca3-g2.crl", "http://ocsp.verisign.com/",
  },
  { "VeriSign Commercial Software Publishers CA",
    "\xdd\x75\x3f\x56\xbf\xbb\xc5\xa1\x7a\x15\x53\xc6\x90\xf9\xfb\xcc", 16,
    "http://crl.verisign.com/Class3SoftwarePublishers.crl",
    "http://ocsp.verisign.com/",
  },
  { "VeriSign Individual Software Publishers CA",
    "\x71\x1f\x0e\x21\xe7\xaa\xea\x32\x3a\x66\x23\xd3\xab\x50\xd6\x69", 16,
    "http://crl.verisign.com/Class2SoftwarePublishers.crl",
    "http://ocsp.verisign.com/",
  },
  { 0, 0, 0, 0, 0 },
};

spc_cacert_t *spc_lookup_cacert(X509 *cert) {
  spc_cacert_t  *entry;
  unsigned int  fingerprint_length;
  unsigned char fingerprint[EVP_MAX_MD_SIZE];

  fingerprint_length = EVP_MAX_MD_SIZE;
  if (!X509_digest(cert, EVP_md5(), fingerprint, &fingerprint_length)) return 0;

  for (entry = lookup_table;  entry->name;  entry++) {
    if (entry->fingerprint_length != fingerprint_length) continue;
    if (!memcmp(entry->fingerprint, fingerprint, fingerprint_length)) return entry;
  }
  return 0;
}
