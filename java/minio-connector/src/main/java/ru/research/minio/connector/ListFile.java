package ru.research.minio.connector;

import io.minio.MinioClient;
import io.minio.ObjectStat;
import io.minio.Result;
import io.minio.errors.*;
import io.minio.messages.Bucket;
import io.minio.messages.Item;
import org.xmlpull.v1.XmlPullParserException;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.List;

public final class ListFile {
    private static final Path DIRECTORY_DOWNLOADS = Paths.get("C:/Users/Andrei.Khlebnikov/git/testing/java/minio-connector/target/d");

    private static String extension(String name, String contentType) {
        if (contentType == null)
            return "";
        int dot = name.lastIndexOf('.');
        if (dot > 0)
            return "";
        int of = contentType.indexOf('/');
        if (of > 0) {
            String subType = contentType.substring(of + 1);
            if (subType.startsWith("octet-stream"))
                return "";
            subType = subType.replace(";", "");
            return "." + subType;
        }
        return "";
    }

    private static void downloadBucket(MinioClient minioClient, String name) throws XmlPullParserException,
            InsufficientDataException, NoSuchAlgorithmException, IOException, NoResponseException, InvalidKeyException,
            InternalException, InvalidBucketNameException, ErrorResponseException, InvalidResponseException,
            InvalidArgumentException {
        Iterable<Result<Item>> objects = minioClient.listObjects(name);
        for (Result<Item> result : objects) {
            Item item = result.get();
            ObjectStat stat = minioClient.statObject(name, item.objectName());
            System.out.println(stat);

            final File newFile;
            String objectFileName = stat.name();
            final String ext = extension(objectFileName, stat.contentType());
            int of = objectFileName.lastIndexOf('/');
            if (of > 0) {
                String filName = objectFileName.substring(of + 1);
                File directory = new File(DIRECTORY_DOWNLOADS.toFile(), objectFileName.substring(0, of));
                directory.mkdirs();
                newFile = new File(directory, filName + ext);
            } else {
                newFile = new File(DIRECTORY_DOWNLOADS.toFile(), objectFileName + ext);
            }
            try (InputStream object = minioClient.getObject(name, item.objectName());
                 OutputStream output = new FileOutputStream(newFile)) {
                byte[] buffer = new byte[4096];
                int read;

                while ((read = object.read(buffer)) > 0) {
                    output.write(buffer, 0, read);
                }
            }
        }
    }

    public static void main(String[] args) throws IOException, XmlPullParserException, NoSuchAlgorithmException,
            RegionConflictException, InvalidKeyException, InvalidPortException, InvalidResponseException,
            ErrorResponseException, NoResponseException, InvalidBucketNameException, InsufficientDataException,
            InvalidEndpointException, InternalException, InvalidArgumentException {
        if (!Files.exists(DIRECTORY_DOWNLOADS))
            Files.createDirectory(DIRECTORY_DOWNLOADS);
        MinioClient minioClient = ConnectionString.minioClient(false);
        List<Bucket> buckets = minioClient.listBuckets();
        for (Bucket bucket : buckets) {
            System.out.println(bucket.name());
        }
        downloadBucket(minioClient, "xrm.smev.exchange");
        downloadBucket(minioClient, "xrm.participant.attachments");
    }
}
