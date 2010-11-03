#ifndef __LOA_H__
#define __LOA_H__

#include <gtk/gtk.h>
#include <webkit/webkit.h>

typedef enum mode_s {
    CMD_MODE,
    ENTRY_MODE,
    INSERT_MODE,
    URL_MODE,
    OPEN_MODE
} mode_t;

typedef struct loa_s {
    void (*init)(void);
    /**/
    int argc;
    char **argv;
    int modal;
    bool shortcuts; /* true when shortcuts appears */
    mode_t mode;
    WebKitWebView *webview;
    GtkLabel *stbarlbl;
    GtkEntry *stbarentry;
    GtkEntry *stbaruri;
    GtkBox *mainvbox;
    GtkWindow *mainwin;
} loa_t;

loa_t *loa_init(int argc, char *argv[]);
void loa_destroy(loa_t *slf);

#endif /* __LOA_H__ */
