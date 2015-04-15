#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "jsmn.h"

#include "json.h"

char URL[] = "https://api.github.com/users/alisdair";
char *KEYS[] = { "name", "location", "public_repos", "hireable", "following_url", "public_gists" };

int main(void)
{
    //char *js = json_fetch(URL);
    /*char js[] = "{"
      "\"following_url\": \"https://api.github.com/users/alisdair/following{/other_user}\","
      "\"name\": \"Alisdair McDiarmid\","
      "\"location\": \"Glasgow\","
      "\"hireable\": true,"
      "\"public_repos\": 49,"
      "\"created_at\": \"2009-03-31T12:36:07Z\","
      "\"updated_at\": \"2015-04-15T09:39:22Z\""
      "}";*/

    char *js = json_fetch_file("test_json.txt");

    char token[64];
    char name[64];
    char location[64];
    char hireable[64];
    char following_url[64];
    int public_repos;
    double public_gists;
    char *str;

    jsmntok_t *tokens = json_tokenise(js);

    typedef enum { START, KEY, PRINT, SKIP, STOP } parse_state;
    parse_state state = START;

    size_t object_tokens = 0;

    for (size_t i = 0, j = 1; j > 0; i++, j--)
    {
        jsmntok_t *t = &tokens[i];

        if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
            j += t->size;

        switch (state)
        {
            case START:
                state = KEY;
                object_tokens = t->size;

                if (object_tokens == 0)
                    state = STOP;
                break;

            case KEY:
                object_tokens--;

                state = SKIP;

                for (size_t i = 0; i < sizeof(KEYS)/sizeof(char *); i++)
                {
                    if (json_token_streq(js, t, KEYS[i]))
                    {
                      printf("%s: ", KEYS[i]);
                      strcpy(token, KEYS[i]);
                      state = PRINT;
                      break;
                    }
                }

                break;

            case SKIP:
                object_tokens--;
                state = KEY;

                if (object_tokens == 0)
                    state = STOP;

                break;

            case PRINT:
                str = json_token_tostr(js, t);

                puts(str);

                if(strcmp("name", token) == 0) // token = string
                {
                    strcpy(name, str);
                }
                if(strcmp("location", token) == 0) // token = string
                {
                    strcpy(location, str);
                }
                if(strcmp("public_repos", token) == 0) // token = int
                {
                    public_repos = atoi(str);
                }
                if(strcmp("hireable", token) == 0) // token = string
                {
                    strcpy(hireable, str);
                }
                if(strcmp("following_url", token) == 0) // token = string
                {
                    strcpy(following_url, str);
                }
                if(strcmp("public_gists", token) == 0) // token = double
                {
                    public_gists = atof(str);
                }

                object_tokens--;
                state = KEY;

                if (object_tokens == 0)
                    state = STOP;

                break;

            case STOP:
                break;

            default:
                break;
        }
    }

    return 0;
}
