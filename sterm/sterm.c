/*
  Copyright (c) 2015 Jakob Nixdorf <flocke@shadowice.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <vte/vte.h>

#include "configuration.h"
#include "defaults-global.h"
#include "terminal.h"

static gchar *config_file = NULL;
static gchar *start_program = NULL;

static GtkWidget *main_window;
static STermTerminal *sterm;
static STermConfig *config;

static GOptionEntry options[] = {
  { "config", 'c', 0, G_OPTION_ARG_FILENAME, &config_file, "Path to the configuration file to use.", NULL },
  { "execute", 'e', 0, G_OPTION_ARG_STRING, &start_program, "Execute the given command.", NULL },
  { NULL }
};

static void sterm_main_exit ()
{
  sterm_configuration_destroy ( config );
  gtk_main_quit ();
}

static void sterm_main_terminal_destroyed_cb ( GtkWidget *terminal )
{
  sterm_main_exit ();
}

static void sterm_main_window_destroyed_cb ( GtkWidget *window )
{
  sterm_terminal_destroy ( sterm );
  sterm_main_exit ();
}

static void sterm_main_title_changed_cb ( GtkWidget *terminal )
{
  const gchar* title = vte_terminal_get_window_title ( VTE_TERMINAL ( terminal ) );
  
  if ( title == NULL )
    title = "STerm";

  gtk_window_set_title ( GTK_WINDOW ( main_window ), title );
}

static gboolean sterm_main_commandline ( int argc, char* argv[] )
{
  gboolean retval = TRUE;
  GError *error = NULL;
  
  GOptionContext *context = g_option_context_new ( "- a simple VTE based terminal emulator" );
  g_option_context_add_main_entries ( context, options, NULL );
  g_option_context_add_group ( context, gtk_get_option_group ( TRUE ) );
  if ( ! g_option_context_parse ( context, &argc, &argv, &error ) ) {
    g_warning ( "ERROR: Failed to parse command-line options: %s\n", error->message );
    retval = FALSE;
  }

  if ( error )
    g_error_free ( error );
  
  return retval;
}

int main ( int argc, char* argv[] )
{
  gtk_init ( &argc, &argv );

  if ( ! sterm_main_commandline ( argc, argv ) )
    return 1;

  if ( config_file == NULL )
    config_file = DEFAULT_CONFIG_FILE;

  config = sterm_configuration_new_from_file ( config_file );

  main_window = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
  gtk_window_set_title ( GTK_WINDOW ( main_window ), "STerm" );
  gtk_container_set_border_width ( GTK_CONTAINER ( main_window ), 0 );

  sterm = sterm_terminal_new ( main_window, config );
  sterm_terminal_setup ( sterm );
  sterm_terminal_start_child ( sterm, start_program );

  g_signal_connect ( G_OBJECT ( main_window ), "destroy", G_CALLBACK ( sterm_main_window_destroyed_cb ), NULL );
  g_signal_connect ( G_OBJECT ( sterm->widget ), "destroy", G_CALLBACK ( sterm_main_terminal_destroyed_cb ), NULL );
  g_signal_connect ( G_OBJECT ( sterm->widget ), "window-title-changed", G_CALLBACK ( sterm_main_title_changed_cb ), NULL );

  gtk_container_add ( GTK_CONTAINER ( main_window ), sterm->widget );

  gtk_widget_show_all ( main_window );
  gtk_main ();

  return 0;
}

