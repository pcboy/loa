/*
 *          DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                  Version 2, December 2004
 *
 *  Copyright (C) 2004 Sam Hocevar
 *  14 rue de Plaisance, 75014 Paris, France
 *  Everyone is permitted to copy and distribute verbatim or modified
 *  copies of this license document, and changing it is allowed as long
 *  as the name is changed.
 *  DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 *
 *
 *  David Hagege <david.hagege@gmail.com>
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#include <gdk/gdkkeysyms.h>

#include "loa.h"


static const gchar clickUrl[] =
"\
    (function () { \
         function clickUrl(urlTag) { \
             var toSearch = 'loa'+urlTag; \
             var urls = document.getElementsByTagName('b'); \
             for (var i = 0; i < urls.length; ++i) { \
                 if (urls[i].getAttribute('name') == toSearch &&\
                     urls[i].getAttribute('class') == 'loaurl') \
                     document.location = urls[i].parentNode.href;\
            } \
          } \
          clickUrl('%s');\
     })()";

static const gchar code[] =
"\
    (function () { \
         function randStr(num) { \
             var chars = 'arstdhneioupyfqw'; \
             var str = ''; \
             for (var i = num; i; i>>=4) { \
                 str += chars[i&0xf]; \
             } \
             return str; \
         } \
     var scripts = document.getElementsByTagName(\"a\"); \
     for (var i = 0; i < scripts.length; i += 1) { \
         var strand = randStr(i+1); \
         scripts[i].innerHTML += \"<b class='loaurl' name=loa\"+strand+\"\
         style=color:red>\"+strand+\"</b>\"; \
     } \
     })()";

static inline void loa_switchMode(loa_t *slf, mode_t m)
{
    slf->mode = m;

    switch (m) {
        case ENTRY_MODE:
            gtk_label_set_text(slf->stbarlbl, "Entry");
            break;
        case CMD_MODE:
            gtk_label_set_text(slf->stbarlbl, "Cmd");
            break;
        case INSERT_MODE:
            gtk_label_set_text(slf->stbarlbl, "Insert");
            break;
        case URL_MODE:
            gtk_label_set_text(slf->stbarlbl, "Url");
            break;
        case OPEN_MODE:
            gtk_label_set_text(slf->stbarlbl, "Open");
            break;
        case SEARCH_MODE:
            gtk_label_set_text(slf->stbarlbl, "Search");
            break;
        default:
            abort();
            break;
    }
}

static void loa_createMainWindow(loa_t *slf)
{
    slf->mainvbox = GTK_BOX(gtk_vbox_new(false, 0)); 
    slf->mainwin = GTK_WINDOW(gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_window_set_default_size(GTK_WINDOW (slf->mainwin), 800, 600);
    gtk_widget_set_name(GTK_WIDGET(slf->mainwin), "mainwin");
    gtk_container_add(GTK_CONTAINER(slf->mainwin), GTK_WIDGET(slf->mainvbox));
}

static void loa_putWebkit(loa_t *slf)
{
    GtkWidget *scrollwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrollwin),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(scrollwin),
            GTK_CORNER_TOP_RIGHT);
    slf->webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(scrollwin), GTK_WIDGET(slf->webview));
    gtk_box_pack_start(slf->mainvbox, scrollwin, true, true, 0);
}

static gint loa_goUri(loa_t *slf, const gchar *txt)
{
    if (txt && strlen(txt)) {
        webkit_web_view_load_uri(slf->webview, txt);
        gtk_entry_set_text(slf->stbaruri, txt);
    }
    return 0;
}

static gint loa_stbarActivateCb(GtkEntry *entry, gpointer data)
{
    loa_t *slf = (loa_t*)data;
    const gchar *txt = gtk_entry_get_text(entry);

    slf->modal = false;
    gtk_widget_grab_focus(GTK_WIDGET(slf->webview));
    switch (slf->mode) {
        case OPEN_MODE:
            loa_goUri(slf, txt);
            break;
        case URL_MODE:
            {
                char tagsearch[1024];
                snprintf(tagsearch, sizeof(tagsearch), clickUrl, txt);
                webkit_web_view_execute_script(slf->webview, tagsearch);
                break;
            }
        case SEARCH_MODE:
            {
                char tosearch[2048];
                int i;
                bool sensitive = false;

                snprintf(tosearch, sizeof(tosearch), "%s", txt);
                for (i = 0; tosearch[i]; ++i)
                    if (isupper(tosearch[i]))
                        sensitive = true;
                if (!webkit_web_view_search_text(slf->webview, tosearch,
                        sensitive, true, true)) {
                    fprintf(stderr, "Search term not found\n");
                }
            }
        default:
            break;
    }
    loa_switchMode(slf, CMD_MODE);
    return 0;
}

static void loa_putStatusBar(loa_t *slf)
{
    GtkHBox *box = GTK_HBOX(gtk_hbox_new(false, 0));
    slf->stbarlbl = GTK_LABEL(gtk_label_new("loa"));
    slf->stbarentry = GTK_ENTRY(gtk_entry_new());
    slf->stbaruri = GTK_ENTRY(gtk_entry_new());

    gtk_editable_set_editable(GTK_EDITABLE(slf->stbaruri), false);
    gtk_entry_set_has_frame(slf->stbarentry, false);
    g_signal_connect(G_OBJECT(slf->stbarentry),
            "activate", G_CALLBACK(loa_stbarActivateCb), slf);
    gtk_box_pack_start(GTK_BOX(box),
            GTK_WIDGET(slf->stbarlbl), false, true, 0);
    gtk_box_pack_start(GTK_BOX(box),
            GTK_WIDGET(slf->stbarentry), false, true, 0);
    gtk_box_pack_start(GTK_BOX(box),
            GTK_WIDGET(slf->stbaruri), true, true, 0);
    gtk_box_pack_start(GTK_BOX(slf->mainvbox),
            GTK_WIDGET(box), false, true, 0);
}

static void loa_getFocusOnBarEntry(loa_t *slf)
{
    slf->modal = true;
    gtk_widget_grab_focus(GTK_WIDGET(slf->stbarentry));
}

static void loa_handleSearch(loa_t *slf)
{
    loa_getFocusOnBarEntry(slf);
}

static void loa_handleEntry(loa_t *slf)
{
    loa_getFocusOnBarEntry(slf);
}

static void loa_handleUrl(loa_t *slf)
{
    loa_getFocusOnBarEntry(slf);
    if (!slf->shortcuts) {
        webkit_web_view_execute_script(slf->webview, code);
        slf->shortcuts = true;
    }
}

static void loa_handleOpen(loa_t *slf)
{
    loa_getFocusOnBarEntry(slf);
}

static void loa_handleStop(loa_t *slf)
{
    webkit_web_view_stop_loading(slf->webview);
}

static void loa_loadFinishedCb(WebKitWebView *view, WebKitWebFrame *frame,
        gpointer data)
{
    loa_t *loa = (loa_t*)data;
    (void)frame;
    gtk_entry_set_text(loa->stbaruri, webkit_web_view_get_uri(view));
    loa->shortcuts = false;
}

static gint loa_loadProgressCb(WebKitWebView *webview,
        gint progress, gpointer data)
{
    loa_t *slf = (loa_t*)data;
    char str[64];

    (void)webview;
    g_snprintf(str, sizeof(str), "Progress:[ %i%%]", progress);
    gtk_window_set_title(slf->mainwin, str);
    return 0;
}

pid_t loa_openNewWin(loa_t *slf, const char *uri)
{
    pid_t pid = fork();

    switch (pid) {
        case 0:
            setsid();
            execlp(slf->argv[0], slf->argv[0], uri, NULL);
            fprintf(stderr, "Can't open new loa instance\n");
            exit(EXIT_FAILURE);
            break;
        case -1:
            fprintf(stderr, "Can't fork\n");
            perror("fork");
            break;
    }
    return pid;
}

static gboolean loa_newWinPolicyDecisionReqCb(WebKitWebView *webview,
        WebKitWebFrame *frame, WebKitNetworkRequest *req,
        WebKitWebNavigationAction *navaction,
        WebKitWebPolicyDecision *policy, gpointer data)
{
    loa_t *slf = (loa_t*)data;
    int button;

    (void)webview, (void)frame, (void)policy;
    button = webkit_web_navigation_action_get_button(navaction);
    if (button == 1 || button == 2) {
        loa_openNewWin(slf, webkit_network_request_get_uri(req));
        return true;
    }
    /* Do nothing if I didn't click on it. */
    return true;
}

static gboolean loa_navPolicyDecisionReqCb(WebKitWebView *webview,
        WebKitWebFrame *frame, WebKitNetworkRequest *req,
        WebKitWebNavigationAction *navaction,
        WebKitWebPolicyDecision *policy, gpointer data)
{
    loa_t *slf = (loa_t*)data;

    (void)webview, (void)frame;

    if (webkit_web_navigation_action_get_button(navaction) == 2) {
        /* middle click on uri */
        loa_openNewWin(slf, webkit_network_request_get_uri(req));
        return true;
    }
    webkit_web_policy_decision_use(policy);
    return true;
}

static gint loa_keyPressCb(GtkWidget *widget,
        GdkEventKey *kevent, gpointer data)
{
    loa_t *slf = (loa_t*)data;

    (void)widget;
    /* g_message("%d, %c;", kevent->keyval, kevent->keyval); */
    gtk_widget_grab_focus(slf->modal ? GTK_WIDGET(slf->stbarentry)
            : GTK_WIDGET(slf->webview));
    if (kevent->type == GDK_KEY_PRESS)  {
        if (slf->mode == CMD_MODE) {
            switch (kevent->keyval) {
                /* TODO: Put all that shit in a config file. */
                case 's':
                    g_message("Stop page");
                    loa_handleStop(slf);
                    break;
                case 'i':
                    g_message("Switch to insert");
                    loa_switchMode(slf, INSERT_MODE);
                    break;
                case 'u':
                    g_message("Switch to url");
                    loa_switchMode(slf, URL_MODE);
                    loa_handleUrl(slf);
                    break;
                case 'e':
                    g_message("Switch to entry");
                    loa_switchMode(slf, ENTRY_MODE);
                    loa_handleEntry(slf);
                    break;
                case 'o':
                    g_message("Switch to open");
                    loa_switchMode(slf, OPEN_MODE);
                    loa_handleOpen(slf);
                    break;
                case 'h': /* Back in colemak */
                    webkit_web_view_go_back(slf->webview);
                    break;
                case 'n': /* Next in colemak */
                    webkit_web_view_go_forward(slf->webview);
                    break;
                case 'd': /* Search in colemak */
                    g_message("Switch to search");
                    loa_switchMode(slf, SEARCH_MODE);
                    loa_handleSearch(slf);
                    break;
                case GDK_Left...GDK_Down: /* arrows */
                    return 0;
            }
            return -1;
        }
        if (kevent->keyval == GDK_Escape) {
            g_message("Switch to cmd");
            loa_switchMode(slf, CMD_MODE);
        }
        switch (slf->mode) {
            case URL_MODE:
            case OPEN_MODE:
                return -1;
            default:
                return 0;
        }
    }

    return 0;
}

static void loa_destroyCb(GtkWidget* widget, gpointer data)
{
    (void)widget;
    (void)data;
    gtk_main_quit();
}

static bool loa_setProxy(const char *uri)
{
    SoupURI *soupuri = NULL;
    SoupSession *sess = webkit_get_default_session();

    if (!uri)
        return false;

    if ((soupuri = soup_uri_new(uri))) {
        g_object_set(sess, "proxy-uri", soupuri, NULL);
        soup_uri_free(soupuri);
        return true;
    }
    return false;
}

static void loa_sigchld(int signum)
{
    (void)signum;
    if (wait(0) < 0)
        perror("wait");
}

loa_t *loa_init(int argc, char **argv)
{
    loa_t *loa = calloc(1, sizeof(loa_t));
    struct sigaction sig;

    loa->argc = argc;
    loa->argv = argv;
    loa->shortcuts = false;
    loa->mode = CMD_MODE;
    loa_createMainWindow(loa);
    loa_putWebkit(loa);
    loa_putStatusBar(loa);

    memset(&sig, '\0', sizeof(struct sigaction));
    sig.sa_handler = loa_sigchld;
    if (sigaction(SIGCHLD, &sig, NULL) < 0)
        perror("sigaction");
    g_signal_connect(G_OBJECT(loa->webview),
            "key_press_event", G_CALLBACK(loa_keyPressCb), loa);
    g_signal_connect(G_OBJECT(loa->webview),
            "load-progress-changed", G_CALLBACK(loa_loadProgressCb), loa);
    g_signal_connect(G_OBJECT(loa->webview),
            "load-finished", G_CALLBACK(loa_loadFinishedCb), loa);
    g_signal_connect(G_OBJECT(loa->webview),
            "navigation-policy-decision-requested",
            G_CALLBACK(loa_navPolicyDecisionReqCb), loa);
    g_signal_connect(G_OBJECT(loa->webview),
            "new-window-policy-decision-requested",
            G_CALLBACK(loa_newWinPolicyDecisionReqCb), loa);
    g_signal_connect (G_OBJECT (loa->webview),
            "destroy", G_CALLBACK (loa_destroyCb), NULL); 

    gtk_widget_show_all(GTK_WIDGET(loa->mainwin));
    if (!loa_setProxy(getenv("LOA_PROXY")))
        loa_setProxy(getenv("http_proxy"));
    loa_goUri(loa, argc > 1 ? argv[1] : "http://google.com");
    return loa;
}

void loa_destroy(loa_t *slf)
{
    free(slf);
}
