#include <iostream>
#include <curl/curl.h>
#include "botan_all.h"
#include "tinyxml2.h"

#define EB_BUFFER_SIZE 4096
static const char *const TAG_NAME_CA = "УдостоверяющийЦентр";

struct elastic_buffer {
    uint8_t *data;
    size_t data_size;
    size_t allocated;
};

static void eb_write(struct elastic_buffer *buf, const char *filename) {
    FILE *fd = fopen(filename, "wb");
    if (fd != nullptr) {
        fwrite(buf->data, buf->data_size, 1, fd);
        fclose(fd);
    }
}

static char *eb_strdup(struct elastic_buffer *buf) {
    if (buf != nullptr && buf->data != nullptr) {
        char *ret = (char *) malloc(buf->data_size + 1);
        memcpy(ret, buf->data, buf->data_size);
        ret[buf->data_size] = 0;
        return ret;
    }
    return nullptr;
}

static void eb_add(struct elastic_buffer *buf, void *buffer, size_t size) {
    if (buf != nullptr) {
        if (buf->data == nullptr) {
            buf->allocated = size + EB_BUFFER_SIZE + 1;
            buf->data = static_cast<uint8_t *>(calloc(1, buf->allocated));
        } else if (buf->data_size + size > buf->allocated) {
            buf->allocated += size + EB_BUFFER_SIZE + 1;
            buf->data = static_cast<uint8_t *>(realloc(buf->data, buf->allocated));
        }
        memcpy(buf->data + buf->data_size, buffer, size);
        buf->data_size += size;
    }
}

static void eb_free(struct elastic_buffer *buf) {
    if (buf != nullptr) {
        if (buf->data != nullptr)
            free(buf->data);
        buf->allocated = 0;
        buf->data_size = 0;
    }
}

static void eb_clean(struct elastic_buffer *buf) {
    if (buf != nullptr) {
        memset(buf->data, 0, buf->allocated);
        buf->data_size = 0;
    }
}

static size_t write_callback(void *buffer, size_t size, size_t nmemb, void *fd) {
    size_t real_size = size * nmemb;
    eb_add((struct elastic_buffer *) fd, buffer, real_size);
    return real_size;
}

static bool download(const char *url, struct elastic_buffer *buf) {
    CURL *curl;
    CURLcode result;
    bool ret = false;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        eb_clean(buf);
        result = curl_easy_perform(curl);
        if (result != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        ret = result == CURLE_OK;
    }
    return ret;
}

static bool download_to(const char *url, const char *filename, struct elastic_buffer *buf) {
    if (download(url, buf)) {
        eb_write(buf, filename);
        return true;
    }
    return false;
}

//class MeinVisitor final : public tinyxml2::XMLVisitor {
//    bool VisitEnter(const tinyxml2::XMLElement &element, const tinyxml2::XMLAttribute *attributes) override {
//        if (strcmp("Название", element.Name()) == 0)
//            fprintf(stdout, "%s = %s\n", element.Name(), element.GetText());
//        return true;
//    }
//};

int main() {
    struct elastic_buffer buf = {nullptr, 0, 0};
//    MeinVisitor visitor;
    std::unique_ptr<Botan::RandomNumberGenerator> rng;

    curl_global_init(CURL_GLOBAL_ALL);
#if defined(BOTAN_HAS_SYSTEM_RNG)
    rng.reset(new Botan::System_RNG);
#else
    rng.reset(new AutoSeeded_RNG);
#endif
    Botan::Certificate_Store_In_SQLite store(":memory:", "", *rng);
    download_to("http://e-trust.gosuslugi.ru/CA/DownloadTSL?schemaVersion=0", "download.xml", &buf);
    auto text = eb_strdup(&buf);
    tinyxml2::XMLDocument doc;
    doc.Parse((char *) buf.data, buf.data_size);
    tinyxml2::XMLElement *root = doc.RootElement();
    if (root != nullptr) {
        auto center = root->FirstChildElement(TAG_NAME_CA);
        for (; center; center = center->NextSiblingElement(TAG_NAME_CA)) {
            auto name = center->FirstChildElement("КраткоеНазвание");
            auto email = center->FirstChildElement("ЭлектроннаяПочта");
            fprintf(stdout, "%s: %s\n", name->GetText(), email->GetText());
        }
//        root->Accept(&visitor);
    }
    free(text);
    eb_free(&buf);
    curl_global_cleanup();
    return 0;
}