#include "desktop_test.h"

gchar *file1 = "/home/test_files/360wallpaper38.jpg";
gchar *file2 = "/home/test_files/001.png";
gchar *rich_dir = "/home/test_files/.deepin_rich_dir_desktop_test";
gchar *app_0 = "/home/test_files/skype.desktop";
gchar *app_1 = "/home/test_files/deepin-user-manual.desktop";

void test_inotify()
{
    void trash_changed();
    Test({
        g_assert(FALSE == desktop_file_filter("snyh.txt"));
        g_assert(TRUE == desktop_file_filter(".snyh.txt"));
        g_assert(TRUE == desktop_file_filter("snyh.txt~"));
    }, "desktop_file_filter");
    Test({
        trash_changed();
    }, "trash_changed");

    void _add_monitor_directory(GFile *f);
    Test({
        GFile *f = g_file_new_for_path(file1);
        _add_monitor_directory(f);
        g_object_unref(f);
    }, " _add_monitor_directory");

    void install_monitor();
    Test({
        install_monitor();
    }, "install_monitor");

    void handle_rename(GFile *, GFile *);
    Test({
        GFile *old_f = g_file_new_for_path(file1);
        gchar *filename = g_strconcat(file1, "2", NULL);
        GFile *new_f = g_file_new_for_path(filename);
        handle_rename(old_f, new_f);

        g_free(filename);
        g_object_unref(old_f);
        g_object_unref(new_f);
    }, "handle_rename");

    void handle_delete(GFile* f);
    Test({
        GFile *f = g_file_new_for_path(file1);
        handle_delete(f);
        if (f != NULL)
            g_object_unref(f);
    }, "handle_delete");

    void handle_update(GFile* f);
    Test({
        GFile *f = g_file_new_for_path(file1);
        handle_update(f);
        g_object_unref(f);
    }, "handle_update");

    void handle_new(GFile* f);
    Test({
        GFile *f = g_file_new_for_path(file1);
        handle_new(f);
        g_object_unref(f);
    }, "handle_new");

    void _remove_monitor_directory(GFile* f);
    Test({
        GFile *f = g_file_new_for_path(file1);
        _remove_monitor_directory(f);
    }, "_remove_monitor_directory");

    void _inotify_poll();
    Test({
        _inotify_poll();
    }, "_inotify_poll");
}

void test_dbus()
{
     /*Test({ */
         /*call_dbus("com.deepin.dde.desktop", "FocusChanged", FALSE); */
     /*}, "desktop_dbus"); */
}

void test_background()
{
    extern GdkWindow* _background_window;
    extern void setup_background_window();
    Test({
         setup_background_window();
         g_object_unref(_background_window);
         _background_window = NULL;
         }, "setup_background_window");
}

void test_background_util()
{
    typedef struct _xfade_data
    {
        //all in seconds.
        gdouble start_time;
        gdouble total_duration;
        gdouble interval;

        cairo_surface_t* fading_surface;
        GdkPixbuf* end_pixbuf;
        gdouble alpha;

        Pixmap pixmap;
    } xfade_data_t;


    GdkScreen *screen = NULL;
    Display *display = NULL;

    screen = gdk_screen_get_default();
    gint number = gdk_screen_get_number(screen);
    gint width = gdk_screen_get_width(screen);
    gint height = gdk_screen_get_height(screen);
    display = XOpenDisplay(gdk_display_get_name(gdk_screen_get_display(screen)));
    Pixmap pixmap = XCreatePixmap (display, RootWindow(display, number), width, height, DefaultDepth(display, number));

    cairo_surface_t *get_surface(Pixmap);
    xfade_data_t *data = g_new0(xfade_data_t, 1);
    data->pixmap = pixmap;
    data->fading_surface = get_surface(pixmap);
    data->end_pixbuf = gdk_pixbuf_new_from_file(file1, NULL);

    // _update_rootpmap Successful.
    /* extern void _update_rootpmap(); */
    /* Test({ */
    /*     _update_rootpmap(pixmap); */
    /* }, "_update_rootpmap"); */

    // on_tick Successful.
    /* extern void on_tick(xfade_data_t *data); */
    /* Test({ */
    /*     on_tick(data); */
    /* }, "on_tick"); */

    // draw_background Successful.
    /* extern void draw_background(xfade_data_t *data); */
    /* Test({ */
    /*     draw_background(data); */
    /* }, "draw_background"); */

    g_object_unref(data->end_pixbuf);
    cairo_surface_destroy(data->fading_surface);
    g_free(data);

    // remove_timers Successful.
    /* extern void remove_timers(); */
    /* Test({ */
    /*     remove_timers(); */
    /* }, "remove_timers"); */

    // get_current_picture_path Successful.
    /* extern const char *get_current_picture_path(); */
    /* Test({ */
    /*     get_current_picture_path(); */
    /* }, "get_current_picture_path"); */

    // get_next_picture_index Successful.
    /* extern guint get_next_picture_index(); */
    /* Test({ */
    /*     get_next_picture_index(); */
    /* }, "get_next_picture_index"); */

    // get_next_picture_path Successful.
    /* extern const char *get_next_picture_path(); */
    /* Test({ */
    /*     get_next_picture_path(); */
    /* }, "get_next_picture_path"); */

    // get_xformed_gdk_pixbuf  Succcessful.
    extern GdkPixbuf *get_xformed_gdk_pixbuf(const char *path);
    /* Test({ */
    /*     const gchar *path = get_current_picture_path(); */
    /*     GdkPixbuf *pixbuf = get_xformed_gdk_pixbuf(path); */
    /*     g_object_unref(pixbuf); */
    /* }, "get_xformed_gdk_pixbuf"); */

    // on_bg_duration_tick Successful.
    extern gboolean on_bg_duration_tick(gpointer data);
    Test({
        on_bg_duration_tick(NULL);
    }, "on_bg_duration_tick");

    // bg_settings_picture_uri_changed
    GSettings *setting = NULL;
    setting = g_settings_new(BG_SCHEMA_ID);

    // haven't test.
    /* Test({ */
    /*     initial_setup(setting); */
    /* }, "initial_setup"); */

    // This function have never used.
    /* extern void bg_settings_picture_uri_changed(GSettings *setting, const gchar *key, gpointer data); */
    /* Test({ */
    /*     bg_settings_picture_uri_changed(setting, BG_PICTURE_URI, NULL); */
    /* }, "bg_settings_picture_uri_changed"); */

    // Test bg_settings_picture_uris_changed Successful.
    /* extern void bg_settings_picture_uris_changed (GSettings *settings, gchar *key, gpointer user_data); */
    /* Test({ */
    /*     bg_settings_picture_uris_changed(setting, BG_PICTURE_URIS, NULL); */
    /* }, "bg_settings_picture_uris_changed"); */

    // Succeessful.
    /* extern void bg_settings_bg_duration_changed (GSettings *, const gchar *, gpointer); */
    /* Test({ */
    /*     bg_settings_bg_duration_changed (setting, BG_BG_DURATION, NULL); */
    /* }, "bg_settings_bg_duration_changed"); */

    // Successful.
    /* extern void bg_settings_xfade_manual_interval_changed (GSettings *, const gchar *, gpointer); */
    /* Test({ */
    /*     bg_settings_xfade_manual_interval_changed (setting, BG_XFADE_MANUAL_INTERVAL, NULL); */
    /* }, "bg_settings_xfade_manual_interval_changed "); */

    // Successful.
    /* extern void bg_settings_xfade_auto_interval_changed(GSettings *, const gchar *, gpointer); */
    /* Test({ */
    /*     bg_settings_xfade_auto_interval_changed(setting, BG_XFADE_AUTO_INTERVAL, NULL); */
    /* }, "bg_settings_xfade_auto_interval_changed"); */

    // Successful.
    /* extern void bg_settings_xfade_auto_mode_changed (GSettings *, const gchar *, gpointer); */
    /* Test({ */
    /*     bg_settings_xfade_auto_mode_changed (setting, BG_XFADE_AUTO_MODE, NULL); */
    /* }, "bg_settings_xfade_auto_mode_changed "); */

    // Successful.
    /* extern void bg_settings_draw_mode_changed (GSettings *, const gchar *, gpointer); */
    /* Test({ */
    /*     bg_settings_draw_mode_changed (setting, BG_DRAW_MODE, NULL); */
    /* }, "bg_settings_draw_mode_changed "); */

    // Successful.
    /* extern void register_account_service_background_path (const gchar *); */
    /* Test({ */
    /*     char* cur_pict = g_settings_get_string (setting, BG_CURRENT_PICT); */
    /*     register_account_service_background_path (cur_pict); */
    /*     g_free(cur_pict); */
    /* }, "register_account_service_background_path "); */

    // Successful.
    /* extern void bg_settings_current_picture_changed (GSettings *, const gchar *, gpointer); */
    /* Test({ */
    /*     bg_settings_current_picture_changed (setting, BG_CURRENT_PICT, NULL); */
    /* }, "bg_settings_current_picture_changed"); */

    GdkPixbuf *get_xformedgdk_pixbuf(const char *path);
    Test({
        GdkPixbuf *pixbuf = get_xformed_gdk_pixbuf(file1);
        g_assert(pixbuf != NULL);
        g_object_unref(pixbuf);
    }, "get_xformed_gdk_pixbuf");

    extern void screen_size_changed_cb(GdkScreen *, gpointer);
    Test({
        GdkScreen *screen = gdk_screen_get_default();
        screen_size_changed_cb(screen, NULL);
    }, "screen_size_changed_cb ");

    g_object_unref(setting);

    // setup_background_timer
    void setup_background_timer();
    Test({
        setup_background_timer();
    }, "setup_background_timer");

    // Error: trance trap!
    void setup_crossfade_timer();
    Test({
        setup_crossfade_timer();
    }, "setup_crossfade_timer");

    // Error: trance trap!
    void setup_timers();
    Test({
        setup_timers();
    }, "setup_timers");

    // parse_picture_uris (gchar * pic_uri)
    void parse_picture_uris (gchar * pic_uri);
    Test({
        parse_picture_uris(file1);
    }, "parse_picture_uris");

}

void test_desktop()
{
    JSObjectRef desktop_get_desktop_entries();
    Test({
        JSObjectRef desktop_get_desktop_entries();
    }, "desktop_get_desktop_entries");

    char* desktop_get_rich_dir_name(GFile* dir);
    Test({
        GFile *f = g_file_new_for_path(rich_dir);
        char *filename = desktop_get_rich_dir_name(f);
        g_free(filename);
        g_object_unref(f);
    }, "desktop_get_rich_dir_name");

    void desktop_set_rich_dir_name(GFile* dir, const char* name);
    Test({
        GFile *f = g_file_new_for_path(rich_dir);
        desktop_set_rich_dir_name(f, "test");
        g_object_unref(f);
    }, "desktop_set_rich_dir_name");

    char* desktop_get_rich_dir_icon(GFile* _dir);
    Test({
        GFile *f = g_file_new_for_path(rich_dir);
        char *filename = desktop_get_rich_dir_name(f);
        g_free(filename);
        g_object_unref(f);
    }, "desktop_get_rich_dir_icon");

    GFile* desktop_create_rich_dir(ArrayContainer fs);
    Test({
        ArrayContainer fs;
        GFile** _files = NULL;
        fs.data = _files;
        fs.num = 2;
        _files[0] = g_file_new_for_commandline_arg(app_0);
        _files[1] = g_file_new_for_commandline_arg(app_1);

        GPtrArray* array = g_ptr_array_new();

        for(size_t i=0; i<fs.num; i++) {
            if (G_IS_DESKTOP_APP_INFO(_files[i])) {
                g_ptr_array_add(array, g_file_new_for_commandline_arg(g_desktop_app_info_get_filename((GDesktopAppInfo*)_files[i])));
            } else {
                g_ptr_array_add(array, g_object_ref(_files[i]));
            }
        }
        ArrayContainer ret;
        ret.num = fs.num;
        ret.data = g_ptr_array_free(array, FALSE);

        GFile *f = desktop_create_rich_dir(ret);
        if (NULL != f)
            g_object_unref(f);
    }, "desktop_create_rich_dir");

    char* desktop_get_desktop_path();
    Test({
        char *path = desktop_get_desktop_path();
        g_free(path);
    }, "desktop_get_desktop_path");

    GFile* _get_useable_file(const char* basename);
    Test({
        GFile *f = _get_useable_file(file1);
        if(f != NULL)
            g_object_unref(f);
    }, "_get_useable_file");

    GFile* desktop_new_file();
    Test({
        GFile *f = desktop_new_file();
        if(NULL != f)
            g_object_unref(f);
    }, "desktop_new_file");

    GFile* desktop_new_directory();
    Test({
        GFile *f = desktop_new_directory();
        if (NULL != f)
            g_object_unref(f);
    }, "desktop_new_directory");


    void dock_config_changed(GSettings* settings, char* key, gpointer usr_data);
    Test({
        #define DOCK_SCHEMA_ID "com.deepin.dde.dock"
        GSettings *dock_gsettings = g_settings_new (DOCK_SCHEMA_ID);
        dock_config_changed(dock_gsettings, "changed::hide-mode", NULL);
        g_object_unref(dock_gsettings);
    }, "dock_config_changed");


    void desktop_config_changed(GSettings* settings, char* key, gpointer usr_data);
    Test({
        #define DESKTOP_SCHEMA_ID "com.deepin.dde.desktop"
        GSettings *desktop_gsettings = g_settings_new (DESKTOP_SCHEMA_ID);
        desktop_config_changed(desktop_gsettings, "changed::show-home-icon", NULL);
        g_object_unref(desktop_gsettings);
    }, "desktop_config_changed");

    gboolean desktop_get_config_boolean(const char* key_name);
    Test({
        desktop_get_config_boolean("changed::show-home-icon");
    }, "desktop_get_config_boolean");

    void screen_change_size(GdkScreen *screen, GdkWindow *w);
    Test({
        extern GtkWidget *container;
        GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(container));
        screen_change_size(screen, gtk_widget_get_window(container));
    }, "screen_change_size");

    void send_lost_focus();
    Test({
        send_lost_focus();
    }, "send_lost_focus");

    void send_get_focus();
    Test({
        send_get_focus();
    }, "send_lost_focus");

    void desktop_emit_webview_ok();
    Test({
        desktop_emit_webview_ok();
    }, "desktop_emit_webview_ok");

    // these functio used in other functions.
    /*void update_workarea_size(GSettings* dock_gsettings);*/
    /*GdkFilterReturn watch_workarea(GdkXEvent *gxevent, GdkEvent* event, gpointer user_data)*/
    /*void unwatch_workarea_changes(GtkWidget* widget)*/
    /*void watch_workarea_changes(GtkWidget* widget, GSettings* dock_gsettings)*/
}

void test_utils()
{
    typedef void Entry;

    void desktop_run_terminal();
    Test({
        desktop_run_terminal();
    }, "desktop_run_terminal");

    void desktop_run_deepin_settings(const char* mod);
    Test({
        desktop_run_deepin_settings("display");
    }, "desktop_run_deepin_settings");

    void desktop_run_deepin_software_center();
    Test({
        desktop_run_deepin_software_center();
    }, "desktop_run_deepin_software_center");

    void desktop_open_trash_can();
    Test({
        desktop_open_trash_can();
    }, "desktop_open_trash_can");

    Entry* desktop_get_home_entry();
    Test({
        Entry *entry = desktop_get_home_entry();
        if (NULL != entry)
            g_object_unref(entry);
    }, "desktop_get_home_entry");

    Entry* desktop_get_computer_entry();
    Test({
        Entry *entry = desktop_get_computer_entry();
        if (NULL != entry)
            g_object_unref(entry);
    }, "desktop_get_computer_entry");

    char* desktop_get_transient_icon (Entry* p1);
    Test({
        GFile *f = g_file_new_for_path(file1);
        gchar *str = (gchar *)desktop_get_transient_icon((Entry *)f);
        g_free(str);
        g_object_unref(f);
    }, "desktop_get_transient_icon");
}

void test_other()
{
    gboolean on_bg_duration_tick(gpointer data);
    Test({
        on_bg_duration_tick(NULL);
    }, "on_bg_duration_tick");

}
