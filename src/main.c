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


#include <stdlib.h>

#include <gtk/gtk.h>
#include <webkit/webkit.h>

#include "loa.h"

int main(int argc, char *argv[])
{
    if (!g_thread_supported ())
        g_thread_init (NULL);

    gtk_init(&argc, &argv);

    loa_t *loa = loa_init(argc, argv);

    gtk_main();
    loa_destroy(loa);
    return EXIT_SUCCESS;
}
