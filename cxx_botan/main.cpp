#if defined(WIN32)
#include <Windows.h>
#endif
#include <WinCryptEx.h>
#include <iostream>
#include <memory>
#include <curl/curl.h>
//#include <botan_all.h>
#include "tinyxml2.h"


#define EB_BUFFER_SIZE 4096
static const char *const TAG_NAME_CA = "УдостоверяющийЦентр";

class Certificate {
private:

};

struct ElasticBuffer {
    uint8_t *data;
    size_t data_size;
    size_t allocated;
};

static void ElasticBuffer_write(struct ElasticBuffer *buf, const char *filename) {
    FILE *fd = fopen(filename, "wb");
    if (fd != nullptr) {
        fwrite(buf->data, buf->data_size, 1, fd);
        fclose(fd);
    }
}

static char *ElasticBuffer_strdup(struct ElasticBuffer *buf) {
    if (buf != nullptr && buf->data != nullptr) {
        char *ret = (char *) malloc(buf->data_size + 1);
        memcpy(ret, buf->data, buf->data_size);
        ret[buf->data_size] = 0;
        return ret;
    }
    return nullptr;
}

static void ElasticBuffer_add(struct ElasticBuffer *buf, void *buffer, size_t size) {
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

static void ElasticBuffer_free(struct ElasticBuffer *buf) {
    if (buf != nullptr) {
        if (buf->data != nullptr)
            free(buf->data);
        buf->allocated = 0;
        buf->data_size = 0;
    }
}

static void ElasticBuffer_clean(struct ElasticBuffer *buf) {
    if (buf != nullptr) {
        memset(buf->data, 0, buf->allocated);
        buf->data_size = 0;
    }
}

static size_t write_callback(void *buffer, size_t size, size_t nmemb, void *fd) {
    size_t real_size = size * nmemb;
    ElasticBuffer_add((struct ElasticBuffer *) fd, buffer, real_size);
    return real_size;
}

static bool ElasticBuffer_download(struct ElasticBuffer *buf, const char *url) {
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

        ElasticBuffer_clean(buf);
        result = curl_easy_perform(curl);
        if (result != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        ret = result == CURLE_OK;
    }
    return ret;
}

static bool ElasticBuffer_file(struct ElasticBuffer *buf, const char *url, const char *filename) {
    if (ElasticBuffer_download(buf, url)) {
        ElasticBuffer_write(buf, filename);
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
    struct ElasticBuffer buf = {nullptr, 0, 0};
//    MeinVisitor visitor;
//    std::unique_ptr<Botan::RandomNumberGenerator> rng;

    curl_global_init(CURL_GLOBAL_ALL);
//#if defined(BOTAN_HAS_SYSTEM_RNG)
//    rng = std::make_unique<Botan::System_RNG>();
//#else
//    rng.reset(new AutoSeeded_RNG);
//#endif
//    Botan::Certificate_Store_In_SQLite store(":memory:", "", *rng);
    ElasticBuffer_file(&buf, "http://e-trust.gosuslugi.ru/CA/DownloadTSL?schemaVersion=0",
                       "ElasticBuffer_download.xml");
    auto text = ElasticBuffer_strdup(&buf);
    tinyxml2::XMLDocument doc;
    doc.Parse((char *) buf.data, buf.data_size);
    auto *root = doc.RootElement();
    if (root != nullptr) {
        auto center = root->FirstChildElement(TAG_NAME_CA);
        for (; center; center = center->NextSiblingElement(TAG_NAME_CA)) {
            auto name = center->FirstChildElement("КраткоеНазвание");
            auto email = center->FirstChildElement("ЭлектроннаяПочта");
            auto compList = center->FirstChildElement("ПрограммноАппаратныеКомплексы");
            if (compList == nullptr)
                continue;
            auto comp = compList->FirstChildElement("ПрограммноАппаратныйКомплекс");
            for (; comp; comp = comp->NextSiblingElement("ПрограммноАппаратныйКомплекс")) {
                auto keys = comp->FirstChildElement("КлючиУполномоченныхЛиц");
                if (keys == nullptr)
                    continue;
                auto key = keys->FirstChildElement("Ключ");
                for (; key; key = key->NextSiblingElement("Ключ")) {
                    auto id = key->FirstChildElement("ИдентификаторКлюча");
                    auto revokeList = key->FirstChildElement("АдресаСписковОтзыва");
                    if (revokeList != nullptr) {
                        auto revokeAddress = revokeList->FirstChildElement("Адрес");
                        for (; revokeAddress; revokeAddress = revokeAddress->NextSiblingElement("Адрес")) {
                            auto address = revokeAddress->GetText();

                        }
                    }
                    auto certificates = key->FirstChildElement("Сертификаты");
                    if (certificates != nullptr) {
                        auto certificate = certificates->FirstChildElement("ДанныеСертификата");
                        for (; certificate; certificate = certificate->NextSiblingElement("ДанныеСертификата")) {
                            auto thump = certificate->FirstChildElement("Отпечаток");
                            auto data = certificate->FirstChildElement("Данные");
                            auto serial = certificate->FirstChildElement("СерийныйНомер");

                        }
                    }
                }
            }
            fprintf(stdout, "%s: %s\n", name->GetText(), email->GetText());
        }
//        root->Accept(&visitor);
    }
    free(text);
    ElasticBuffer_free(&buf);
    curl_global_cleanup();
    return 0;
}