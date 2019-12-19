package ru.research.minio.connector;

import io.minio.MinioClient;
import io.minio.errors.*;
import okhttp3.*;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.Proxy;
import java.net.URL;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.Properties;
import java.util.concurrent.TimeUnit;

import static ru.research.minio.connector.Constants.*;

public final class ConnectionString {

    static Authenticator authenticator;
    static String proxyHostname;
    static int proxyPort;

    static {
        URL resource = UploadFile.class.getClassLoader().getResource(".proxy");
        if (resource != null) {
            try (InputStream stream = resource.openStream()) {
                final Properties properties = new Properties();
                properties.load(stream);

                proxyHostname = properties.getProperty("hostname");
                proxyPort = Integer.parseInt(properties.getProperty("port", "3128"));
                System.setProperty("http.proxyHost", proxyHostname);
                System.setProperty("http.proxyPort", "" + proxyPort);
                final String username = properties.getProperty("username");
                if (username != null && !username.isEmpty()) {
                    authenticator = new Authenticator() {
                        @Override
                        public Request authenticate(Route route, Response response) {
                            String credential = Credentials.basic(username, properties.getProperty("password"));
                            return response.request().newBuilder()
                                    .header("Proxy-Authorization", credential)
                                    .build();
                        }
                    };
                }
            } catch (IOException e) {
                throw new IllegalArgumentException(e);
            }
        }
    }

    private ConnectionString() {
    }

    public static OkHttpClient httpClient() {
        OkHttpClient.Builder builder = new OkHttpClient.Builder()
                .connectTimeout(60, TimeUnit.SECONDS)
                .writeTimeout(60, TimeUnit.SECONDS)
                .readTimeout(60, TimeUnit.SECONDS);
        if (ConnectionString.authenticator != null) {
            builder.proxy(new Proxy(Proxy.Type.HTTP, new InetSocketAddress(ConnectionString.proxyHostname, ConnectionString.proxyPort)))
                    .proxyAuthenticator(ConnectionString.authenticator);
        }
        return builder.build();
    }

    public static MinioClient minioClient(boolean trace) throws InvalidPortException, InvalidEndpointException, IOException,
            InvalidKeyException, NoSuchAlgorithmException, InsufficientDataException, InvalidResponseException, InternalException,
            NoResponseException, InvalidBucketNameException, XmlPullParserException, ErrorResponseException, RegionConflictException {
        MinioClient minioClient = new MinioClient(
                MINIO_HOSTNAME, MINIO_PORT, MINIO_ACCESS_KEY, MINIO_SECRET_KEY, null, false, ConnectionString.httpClient());
        if (trace)
            minioClient.traceOn(System.err);
        boolean exists = minioClient.bucketExists(MINIO_BUCKET);
        if (exists) {
            System.out.println("Bucket already exists.");
        } else {
            minioClient.makeBucket(MINIO_BUCKET);
        }
        return minioClient;
    }
}
