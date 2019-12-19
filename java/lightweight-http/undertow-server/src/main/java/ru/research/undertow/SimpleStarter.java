package ru.research.undertow;

import com.google.common.hash.Hashing;
import com.google.common.io.ByteStreams;
import io.undertow.Handlers;
import io.undertow.Undertow;
import io.undertow.io.Sender;
import io.undertow.server.DefaultResponseListener;
import io.undertow.server.HttpHandler;
import io.undertow.server.HttpServerExchange;
import io.undertow.server.handlers.PathHandler;
import io.undertow.server.handlers.form.FormData;
import io.undertow.server.handlers.form.FormDataParser;
import io.undertow.server.handlers.form.FormParserFactory;
import io.undertow.util.AttachmentKey;
import io.undertow.util.Headers;
import io.undertow.util.Methods;
import org.apache.log4j.Logger;
import org.bouncycastle.asn1.x500.AttributeTypeAndValue;
import org.bouncycastle.asn1.x500.RDN;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x509.AlgorithmIdentifier;
import org.bouncycastle.cert.X509CertificateHolder;
import org.bouncycastle.cms.CMSSignedData;
import org.bouncycastle.cms.SignerInformation;
import org.bouncycastle.cms.SignerInformationStore;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.operator.DefaultSignatureAlgorithmIdentifierFinder;
import org.bouncycastle.operator.SignatureAlgorithmIdentifierFinder;
import org.bouncycastle.util.Store;
import org.json.JSONArray;
import org.json.JSONObject;

import java.security.Security;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public final class SimpleStarter {
    private static final String SIGNATURE_KEY = "signature";
    private static final String MESSAGE_KEY = "message";
    private static final String CERTIFICATE_KEY = "certificate";
    private static final String CODE_KEY = "code";
    private static final String VALID_KEY = "valid";
    private static final String SIGNATURES_KEY = "signatures";
    private static final String CERTIFICATES_KEY = "certificates";
    private static final String FORMAT_KEY = "format";
    private static final String TYPE_KEY = "type";
    private static final String APPLICATION_JSON_CHARSET_UTF_8 = "application/json;charset=utf-8";
    private static final AttachmentKey<Throwable> THROWABLE_ATTACHMENT_KEY = AttachmentKey.create(Throwable.class);

    public static void main(String[] args) {
        Security.addProvider(new BouncyCastleProvider());
        PathHandler path = Handlers.path(Handlers.redirect("/"));
        path.addPrefixPath("/cms/verify", new CMSVerificationHttpHandler());
        Undertow server = Undertow.builder()
                .addHttpListener(8090, "127.0.0.1")
                .setWorkerThreads(1)
                .setIoThreads(1)
                .setHandler(path).build();
        server.start();
    }

    private enum x509NameConstant {
        OID_ORGN("1.2.643.100.1", "ogrn"),
        OID_INN("1.2.643.3.131.1.1", "inn"),
        OID_SNILS("1.2.643.100.3", "snils"),
        OID_OGRNIP("1.2.643.100.5", "ogrnip"),
        OID_RNS_FSS("1.2.643.3.141.1.1", "rnsFss"),
        OID_KP_FSS("1.2.643.3.141.1.2", "kpFss"),
        OID_EMAIL("1.2.840.113549.1.9.1", "email"),
        OID_PSEUDONYM("2.5.4.65", "pseudonym"),
        OID_POST_ADDRESS("2.5.4.16", "postAddress"),
        OID_COMMON_NAME("2.5.4.3", "commonName"),
        OID_SUR_NAME("2.5.4.4", "surname"),
        OID_DEVICE_SERIAL_NUMBER("2.5.4.5", "deviceSerialNumber"),
        OID_COUNTRY_NAME("2.5.4.6", "countryName"),
        OID_LOCALITY_NAME("2.5.4.7", "localityName"),
        OID_STATE_OR_PROVINCE_NAME("2.5.4.8", "stateOrProvinceName"),
        OID_STREET_ADDRESS("2.5.4.9", "streetAddress"),
        OID_ORGANIZATION_NAME("2.5.4.10", "organizationName"),
        OID_ORGANIZATIONAL_UNIT_NAME("2.5.4.11", "organizationalUnitName"),
        OID_TITLE("2.5.4.12", "title"),
        OID_DESCRIPTION("2.5.4.13", "description"),
        OID_SEARCH_GUIDE("2.5.4.14", "searchGuide"),
        OID_BUSINESS_CATEGORY("2.5.4.15", "businessCategory"),
        OID_POSTAL_ADDRESS("2.5.4.16", "postalAddress"),
        OID_POSTAL_CODE("2.5.4.17", "postalCode"),
        OID_POST_OFFICE_BOX("2.5.4.18", "postOfficeBox"),
        OID_PHYSICAL_DELIVERY_OFFICE_NAME("2.5.4.19", "physicalDeliveryOfficeName"),
        OID_TELEPHONE_NUMBER("2.5.4.20", "telephoneNumber"),
        OID_TELEX_NUMBER("2.5.4.21", "telexNumber"),
        OID_TELETEXT_TERMINAL_IDENTIFIER("2.5.4.22", "teletextTerminalIdentifier"),
        OID_FACSIMILE_TELEPHONE_NUMBER("2.5.4.23", "facsimileTelephoneNumber"),
        OID_X21_ADDRESS("2.5.4.24", "x21Address"),
        OID_INTERNATIONAL_ISDN_NUMBER("2.5.4.25", "internationalISDNNumber"),
        OID_REGISTERED_ADDRESS("2.5.4.26", "registeredAddress"),
        OID_DESTINATION_INDICATOR("2.5.4.27", "destinationIndicator"),
        OID_PREFERRED_DELIVERY_METHOD("2.5.4.28", "preferredDeliveryMethod"),
        OID_PRESENTATION_ADDRESS("2.5.4.29", "presentationAddress"),
        OID_SUPPORTED_APPLICATION_CONTEXT("2.5.4.30", "supportedApplicationAddress"),
        OID_MEMBER("2.5.4.31", "member"),
        OID_OWNER("2.5.4.32", "owner"),
        OID_ROLE_OCCUPANT("2.5.4.33", "roleOccupant"),
        OID_SEE_ALSO("2.5.4.34", "seeAlso"),
        OID_USER_PASSWORD("2.5.4.35", "userPassword"),
        OID_USER_CERTIFICATE("2.5.4.36", "userCertificate"),
        OID_CA_CERTIFICATE("2.5.4.37", "caCertificate"),
        OID_AUTHORITY_REVOCATION_LIST("2.5.4.38", "authorityRevocationList"),
        OID_CERTIFICATE_REVOCATION_LIST("2.5.4.39", "certificateRevocationList"),
        OID_CROSS_CERTIFICATE_PAIR("2.5.4.40", "crossCertificatePair"),
        OID_GIVEN_NAME("2.5.4.42", "givenName"),
        OID_INITIALS("2.5.4.43", "initials"),
        OID_DN_QUALIFIER("2.5.4.46", "dnQualifier");
        private final String id;
        private final String visibleName;

        x509NameConstant(String id, String visibleName) {
            this.id = id;
            this.visibleName = visibleName;
        }
    }

    private static final class ExceptionResponseListener implements DefaultResponseListener {
        @Override
        public boolean handleDefaultResponse(HttpServerExchange exchange) {
            if (!exchange.isResponseChannelAvailable()) {
                return false;
            }
            if (exchange.getStatusCode() == 500) {
                JSONObject object = new JSONObject();
                Throwable throwable = exchange.getAttachment(THROWABLE_ATTACHMENT_KEY);
                object.put(CODE_KEY, "GEP-00000000");
                object.put(MESSAGE_KEY, throwable == null ? "Неизвестная ошибка" : throwable.getLocalizedMessage());
                object.put(VALID_KEY, false);
                object.put(SIGNATURES_KEY, new JSONObject("{}"));
                exchange.setStatusCode(200);
                exchange.getResponseHeaders().put(Headers.CONTENT_TYPE, APPLICATION_JSON_CHARSET_UTF_8);
                Sender sender = exchange.getResponseSender();
                sender.send(object.toString(2));
                return true;
            }
            return false;
        }
    }

    private static final class MultipartVerificationReader implements HttpHandler {
        private static final Logger LOGGER = Logger.getLogger(MultipartVerificationReader.class);
        private static final Map<String, x509NameConstant> X509_NAME_CONSTANTS = new HashMap<String, x509NameConstant>() {
            {
                for (x509NameConstant constant : x509NameConstant.values()) {
                    put(constant.id, constant);
                }
            }
        };
        private static final SignatureAlgorithmIdentifierFinder SIGNATURE_ALGORITHM_IDENTIFIER_FINDER =
                new DefaultSignatureAlgorithmIdentifierFinder();
        private final Lock writeLock;
        private final Lock readLock;
        private final FormParserFactory.ParserDefinition multipartParser;

        MultipartVerificationReader(FormParserFactory.ParserDefinition multipartParser) {
            this.multipartParser = multipartParser;
            ReentrantReadWriteLock lock = new ReentrantReadWriteLock();
            this.writeLock = lock.writeLock();
            this.readLock = lock.readLock();
        }

        private static JSONObject parseX500(X500Name name) {
            RDN[] rdNs = name.getRDNs();
            JSONObject object = new JSONObject();
            if (rdNs != null) {
                for (RDN rdn : rdNs) {
                    AttributeTypeAndValue tv = rdn.getFirst();
                    x509NameConstant constant = X509_NAME_CONSTANTS.get(tv.getType().getId());
                    String value = tv.getValue().toString();
                    if (constant == null) {
                        object.put(tv.getType().getId(), value);
                    } else {
                        object.put(constant.visibleName, value);
                    }
                }
            }
            return object;
        }

        @Override
        public void handleRequest(HttpServerExchange exchange) throws Exception {
            writeLock.lock();
            try (FormDataParser parser = multipartParser.create(exchange)) {
                FormData data = parser.parseBlocking();
                FormData.FileItem message = data.getFirst(MESSAGE_KEY).getFileItem();
                FormData.FileItem signature = data.getFirst(SIGNATURE_KEY).getFileItem();
                CMSSignedData cmsSignedData = new CMSSignedData(signature.getInputStream());
                Store<X509CertificateHolder> certificates = cmsSignedData.getCertificates();
                SignerInformationStore signerInfos = cmsSignedData.getSignerInfos();
                JSONObject object = new JSONObject();
                JSONArray signaturesObject = new JSONArray();
                byte[] messageContent = ByteStreams.toByteArray(message.getInputStream());
                for (SignerInformation information : signerInfos.getSigners()) {
                    @SuppressWarnings({"unchecked"})
                    Collection<X509CertificateHolder> matches = (Collection<X509CertificateHolder>) certificates.getMatches(information.getSID());
                    if (matches.isEmpty())
                        continue;
                    JSONArray certificatesObject = new JSONArray();
                    byte[] currentCertificate;
                    for (X509CertificateHolder signerCertificate : matches) {
                        currentCertificate = signerCertificate.getEncoded();
                        JSONObject certificateObject = new JSONObject();
                        certificateObject.put("serialNumber", signerCertificate.getSerialNumber());
                        certificateObject.put("version", signerCertificate.getVersionNumber());
                        certificateObject.put("startDate", signerCertificate.getNotBefore());
                        certificateObject.put("endDate", signerCertificate.getNotAfter());
                        certificateObject.put("subject", parseX500(signerCertificate.getSubject()));
                        certificateObject.put("issuer", parseX500(signerCertificate.getIssuer()));
                        certificateObject.put("thumbprint", Hashing.sha1().hashBytes(currentCertificate).toString());
                        certificatesObject.put(certificateObject);
                        byte[] signatureContent = signerCertificate.getSignature();
                        AlgorithmIdentifier signatureAlgorithm = signerCertificate.getSignatureAlgorithm();
                        boolean isVerified = verification(messageContent, currentCertificate, signatureContent, signatureAlgorithm);
                        LOGGER.debug("Verified[" + signatureAlgorithm.getAlgorithm().getId() + "]: " + isVerified);
                    }

                    JSONObject signatureObject = new JSONObject();
                    signatureObject.put(TYPE_KEY, "StrongQualified");
                    signatureObject.put(FORMAT_KEY, "CMS");
                    signatureObject.put(VALID_KEY, true);
                    signatureObject.put(CERTIFICATES_KEY, certificatesObject);
                    signaturesObject.put(signatureObject);
                }
                exchange.getResponseHeaders().put(Headers.CONTENT_TYPE, APPLICATION_JSON_CHARSET_UTF_8);
                object.put(VALID_KEY, true);
                object.put(SIGNATURES_KEY, signaturesObject);
                exchange.getResponseSender().send(object.toString(2));
            } finally {
                writeLock.unlock();
            }
        }

        private boolean verification(byte[] content, byte[] certificate, byte[] signature,
                                     AlgorithmIdentifier signatureAlgorithm) {
            return false;
        }
    }

    private static final class CMSVerificationHttpHandler implements HttpHandler {
        private static final Logger LOGGER = Logger.getLogger(CMSVerificationHttpHandler.class);
        private static final AtomicInteger INDEX = new AtomicInteger(0);
        private static final ThreadLocal<Integer> THREAD_LOCAL_INDEX = ThreadLocal.withInitial(INDEX::incrementAndGet);
        private final DefaultResponseListener responseListener = new ExceptionResponseListener();
        private final MultiPartRawParser multipartParser = new MultiPartRawParser();
        private final MultipartVerificationReader reader = new MultipartVerificationReader(multipartParser);

        @Override
        public void handleRequest(HttpServerExchange exchange) throws Exception {
            LOGGER.debug(String.format("[%d] Thread", THREAD_LOCAL_INDEX.get()));
            exchange.addDefaultResponseListener(responseListener);
            if (!Methods.POST.equals(exchange.getRequestMethod())) {
                throw new UnsupportedOperationException("Только метод POST поддерживается");
            }
            try (FormDataParser parser = multipartParser.create(exchange)) {
                parser.parse(reader);
            }
        }

        void destroy() {
            THREAD_LOCAL_INDEX.remove();
        }
    }

}
