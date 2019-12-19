package ru.research.minio.connector;

import io.minio.MinioClient;
import io.minio.errors.*;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.HashMap;
import java.util.Map;

import static ru.research.minio.connector.Constants.MINIO_BUCKET;

public final class UploadFile {

    public static void main(String[] args) throws InvalidPortException, InvalidEndpointException, IOException, InvalidKeyException,
            NoSuchAlgorithmException, InsufficientDataException, InvalidResponseException, InternalException, NoResponseException,
            InvalidBucketNameException, XmlPullParserException, ErrorResponseException, RegionConflictException, InvalidArgumentException {
        MinioClient minioClient = ConnectionString.minioClient(true);
        Map<String, String> headers = new HashMap<>();
        headers.put("owner", "Pastor");
        minioClient.putObject(MINIO_BUCKET, "gitscan.exe",
                "C:\\Users\\Andrei.Khlebnikov\\git\\gitscan.exe", null,
                headers, null, "application/octet-stream");
    }
}
