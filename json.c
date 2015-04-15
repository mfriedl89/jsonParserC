#include <curl/curl.h>
#include <stdlib.h>

#include "json.h"
#include "buf.h"

#define BUFFER_SIZE 32768
#define JSON_TOKENS 256

static size_t fetch_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    buf_t *buf = (buf_t *) userp;
    size_t total = size * nmemb;

    if (buf->limit - buf->len < total)
    {
        buf = buf_size(buf, buf->limit + total);
    }

    buf_concat(buf, buffer, total);

    return total;
}

char * json_fetch(char *url)
{
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);

    buf_t *buf = buf_size(NULL, BUFFER_SIZE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "jsmn-example (https://github.com/alisdair/jsmn-example, alisdair@mcdiarmid.org)");

    struct curl_slist *hs = curl_slist_append(NULL, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);

    char *js = buf_tostr(buf);
    free(buf->data);
    free(buf);

    return js;
}

jsmntok_t * json_tokenise(char *js)
{
    jsmn_parser parser;
    jsmn_init(&parser);

    unsigned int n = JSON_TOKENS;
    jsmntok_t *tokens = malloc(sizeof(jsmntok_t) * n);

    int ret = jsmn_parse(&parser, js, tokens, n);

    while (ret == JSMN_ERROR_NOMEM)
    {
        n = n * 2 + 1;
        tokens = realloc(tokens, sizeof(jsmntok_t) * n);
        ret = jsmn_parse(&parser, js, tokens, n);
    }

    return tokens;
}

bool json_token_streq(char *js, jsmntok_t *t, char *s)
{
    return (strncmp(js + t->start, s, t->end - t->start) == 0
            && strlen(s) == (size_t) (t->end - t->start));
}

char * json_token_tostr(char *js, jsmntok_t *t)
{
    js[t->end] = '\0';
    return js + t->start;
}

char * json_fetch_file(char *file_path)
{
  FILE *file_in;
  long int file_size;
  size_t read_size;

  // argument for config file not set
  file_in = fopen(file_path, "r");

  if(file_in == NULL)
  {
    printf("no file found\n");
    return "";
  }

  fseek(file_in, 0, SEEK_END); // go to last position in file
  file_size = ftell(file_in); // get current position in stream (=size)
  rewind(file_in); // set position of stream to the beginning

  if(file_size == 0)
  {
    // file does not exist
    printf("no file found\n");
    return "";
  }

  //allocate memory for file buffer
  char * buffer = (char*) malloc(sizeof(char) * file_size + 1 * sizeof(char));

  if(buffer == NULL)
  {
    free(buffer);
    if(file_in != NULL)
    {
      fclose(file_in);
    }
    return "";
  }

  buffer[file_size] = '\0';

  // copy content of file to buffer
  read_size = fread(buffer, sizeof(char), file_size, file_in);

  fclose(file_in);

  if(read_size != file_size)
  {
    // read error
    printf("no file found\n");
    return "";
  }

  return buffer;
}
